#include "combination.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <exception>
#include <vector>
#include <string>
#include <functional>
using namespace std;

class test_exception: public std::exception
{
  std::string message;
public:
  test_exception (const char* msg)
    : message(msg)
  {
  }
  virtual ~test_exception() throw ()
  {
  }

  virtual const char* what() const throw()
  {
    return message.c_str();
  }
};

#define throw_except(msg) do {				\
    cout << "except at LINE:" << __LINE__ << endl;	\
    throw test_exception(msg);				\
  }							\
  while (false)


template <typename Iter, typename Comp>
bool
test_sequence_grow (Iter first, Iter last, Comp comp)
{
  if (first == last)
    return true;

  for (Iter a = first, b = first; ++b != last; ++a)
    {
      if (comp (*b, *a))
	return false;
    }
  return true;
}

// warning: sequence1 < sequence2 may be differenct with sequence2 > sequence1
//          because sequence1's comp object may be differenct with sequence2's
template <typename Iter, 
	  typename Comp = std::less<typename std::iterator_traits<Iter>::value_type> >
class sequence
{
  typedef typename iterator_traits<Iter>::value_type value_type;

  vector<value_type> values;

  Comp compare;
public:
  typedef typename vector<value_type>::const_iterator iterator;
  
  explicit
  sequence (Comp comp = Comp())
    : compare(comp)
  {
  }

  template <typename Iter2>
  sequence (Iter2 begin, Iter2 end, Comp comp = Comp())
    : values(begin, end), compare(comp)
  {
  }

  template <typename Iter2, typename Comp2>
  sequence&
  operator = (const sequence<Iter2, Comp2>& seq)
  {
    values.assign (seq.begin(), seq.end());
    return *this;
  }

  template <typename Iter2>
  void
  assign (Iter2 begin, Iter2 end)
  {
    values.assign (begin, end);
  }
  
  template <typename Iter2, typename Comp2>
  bool
  operator == (const sequence<Iter2, Comp2>& seq) const
  {
    return (!(*this < seq) && !(seq < *this));
  }
  
  template <typename Iter2, typename Comp2>
  bool
  operator != (const sequence<Iter2, Comp2>& seq) const
  {
    return !(*this == seq);
  }
  
  template <typename Iter2, typename Comp2>
  bool
  operator < (const sequence<Iter2, Comp2>& seq) const
  {
    return std::lexicographical_compare (begin(), end(),
					 seq.begin(), seq.end(), 
					 compare);
  }
  template <typename Iter2, typename Comp2>
  bool
  operator > (const sequence<Iter2, Comp2>& seq) const
  {
    return std::lexicographical_compare (seq.begin(), seq.end(),
					 begin(), end(),
					 compare);
  }
  template <typename Iter2, typename Comp2>
  bool
  operator <= (const sequence<Iter2, Comp2>& seq) const
  {
    return !(*this > seq);
  }

  template <typename Iter2, typename Comp2>
  bool
  operator >= (const sequence<Iter2, Comp2>& seq) const
  {
    return !(*this < seq);
  }

  iterator
  begin() const
  {
    return values.begin();
  }

  iterator
  end() const
  {
    return values.end();
  }

  void
  clear ()
  {
    values.clear ();
  }

  void
  out () const
  {
    for (iterator i = begin(); i != end(); ++i)
      cout << *i << " ";
    cout << endl;
  }
};

template <typename seq_t>
bool seq_cmp_eq(const seq_t& s1, const seq_t& s2)
{
  if (!(s1 == s2))
    {
      s1.out ();
      s2.out ();

      return false;
    }
  return true;
}

template <typename seq_t>
bool seq_cmp_lt(const seq_t& s1, const seq_t& s2)
{
  if (!(s1 < s2))
    {
      s1.out ();
      s2.out ();

      return false;
    }
  return true;
}

template <typename seq_t>
bool seq_cmp_le(const seq_t& s1, const seq_t& s2)
{
  if (!(s1 <= s2))
    {
      s1.out ();
      s2.out ();

      return false;
    }
  return true;
}



template <typename Iter, typename Compare>
class test_base
{
protected:
  Compare comp;

  test_base(Compare _comp)
    : comp(_comp)
  {
  }

public:
  typedef typename std::iterator_traits<Iter>::value_type value_type;

public:
  virtual Iter begin() const = 0;
  virtual Iter end() const = 0;

  virtual Iter data_begin() const = 0;
  virtual Iter data_end() const = 0;

  virtual void init (bool min) = 0;

  virtual bool next () = 0;
  virtual bool prev () = 0;

protected:
  virtual bool self_check () const = 0;

protected:
  typedef sequence<Iter, Compare> seq_t;

  inline seq_t value() const
  {
    return seq_t(begin(), end(), comp);
  }
  inline seq_t data() const
  {
    return seq_t(data_begin(), data_end(), comp);
  }

public:
  void test ()
  {
    init (true);
    seq_t min_value = value();
    seq_t min_data = data();

    init (false);
    seq_t max_value = value();
    seq_t max_data = data();

    // min and max values
    {
      init (true);
      if (prev () != false)
	throw_except("prev return error!!");
      if (!seq_cmp_eq(max_value,value()))
	throw_except("min prev to max, `value\' error");
      if (!seq_cmp_eq(max_data, data()))
	throw_except("min prev to max, `data\' error");

      init (false);
      if (next () != false)
	throw_except("next return error!!");
      if (!seq_cmp_eq(min_value, value()))
	throw_except("max next to min, `value\' error");
      if (!seq_cmp_eq(min_data, data()))
	throw_except("max next to min, `data\' error");
    }

    std::vector<seq_t> value_seq;

    init (true);
    do
      {
	if (!self_check())
	  {
	    data().out ();
	    throw_except ("self check failed");
	  }

	seq_t v = value();
	if (!value_seq.empty())
	  {
	    if (!seq_cmp_lt(value_seq.back(), v))
	      throw_except("next don't inc");
	  }

	value_seq.push_back (v);
      }
    while (next ());

    // head and tail
    {
      if (!seq_cmp_eq(value_seq.front(), min_value))
	throw_except("first seq don't equal to min");
      if (!seq_cmp_eq(value_seq.back(), max_value))
	throw_except("last seq don't equal to max");
    }

    // test min value/data
    {
      if (!seq_cmp_eq(value(), min_value))
	throw_except("next finish, `value\' don't equal to min");
      if (!seq_cmp_eq(data(), min_data))
	throw_except("next finish, `data\' don't equal to min");
    }

    size_t total_len = value_seq.size();

    if (prev () != false)
      throw_except("prev return error!!");
    
    // test max value/data
    {
      if (!seq_cmp_eq(value(), max_value))
	throw_except("min prev to max, `value\' error");
      if (!seq_cmp_eq(data(), max_data))
	throw_except("min prev to max, `data\' error");
    }

    typename std::vector<seq_t>::const_iterator ni = value_seq.end();

    do
      {
	if (!self_check())
	  {
	    data().out ();
	    throw_except ("self check failed");
	  }

	if (ni == value_seq.begin())
	  throw_except("number of prev seq big than next");

	seq_t v = value();

	if (ni != value_seq.end())
	  {
	    if (!seq_cmp_lt(v, *ni))
	      throw_except("prev don't dec");
	  }

	--ni;
	if (!seq_cmp_eq(v, *ni))
	  throw_except("prev don't equal next");
      }
    while (prev ());

    // test size equal
    if (ni != value_seq.begin())
      throw_except("number of prev less than next");

    // test max value
    {
      if (!seq_cmp_eq(value(), max_value))
	throw_except("prev finish, `value\' don't equal max");
      if (data() != max_data)
	throw_except("prev finish, `data\' don't equal max");
    }

    // test next and prev by any order
    {
      seq_t cur_value = value();
      seq_t cur_data = data();
      for (size_t i = 0; i < total_len; ++i)
	{
	  next ();
	  seq_t next_value = value();
	  seq_t next_data = data ();
	  
	  prev ();
	  seq_t prev_value = value();
	  seq_t prev_data = data();

	  if (!seq_cmp_eq(cur_value, prev_value))
	    throw_except("prev of next, `value\' error");
	  if (!seq_cmp_eq(cur_data, prev_data))
	    throw_except("prev of next, `data\' error");

	  next ();
	  cur_value = value();
	  cur_data = data();

	  if (!seq_cmp_eq(cur_value, next_value))
	    throw_except("next of prev of next, `value\' error");
	  if (!seq_cmp_eq(cur_data, next_data))
	    throw_except("next of prev of next, `data\' error");
	}
    }
  }

  template <typename Iter2, typename Compare2>
  void equal_test (test_base<Iter2, Compare2> &second)
  {
  }
};

template <typename Iter, 
	  typename Compare = std::less<typename std::iterator_traits<Iter>::value_type> >
class test_partial_permutation: public test_base<Iter, Compare>
{
  Iter first;
  Iter middle;
  Iter last;

  using test_base<Iter, Compare>::comp;

public:
  test_partial_permutation (Iter _first, Iter _middle, Iter _last, 
			    Compare _comp = Compare())
    : test_base<Iter, Compare>(_comp),
      first(_first), middle(_middle), last(_last)
  {
  }
  
  virtual Iter begin() const
  {
    return first;
  }
  virtual Iter end() const
  {
    return middle;
  }
  virtual Iter data_begin() const
  {
    return first;
  }
  virtual Iter data_end() const
  {
    return last;
  }

  virtual void init (bool min)
  {
    std::sort(first, last, comp);
    if (!min)
      {
	std::reverse (first, last);
	std::reverse (middle, last);
      }
  }

  virtual bool next ()
  {
    return boost::next_partial_permutation (first, middle, last, comp);
  }
  virtual bool prev ()
  {
    return boost::prev_partial_permutation (first, middle, last, comp);
  }
protected:
  virtual bool self_check () const
  {
    return test_sequence_grow (middle, last, comp);
  }
};


template <typename Iter, 
	  typename Compare = std::less<typename std::iterator_traits<Iter>::value_type> >
class test_combination: public test_base<Iter, Compare>
{
  Iter first;
  Iter middle;
  Iter last;

  using test_base<Iter, Compare>::comp;

public:
  test_combination (Iter _first, Iter _middle, Iter _last, 
		    Compare _comp = Compare())
    : test_base<Iter, Compare>(_comp),
      first(_first), middle(_middle), last(_last)
  {
  }
  
  virtual Iter begin() const
  {
    return first;
  }
  virtual Iter end() const
  {
    return middle;
  }
  virtual Iter data_begin() const
  {
    return first;
  }
  virtual Iter data_end() const
  {
    return last;
  }

  virtual void init (bool min)
  {
    std::sort(first, last, comp);
    if (!min)
      {
	std::reverse (first, last);
	std::reverse (middle, last);
	std::reverse (first, middle);
      }
  }

  virtual bool next ()
  {
    return boost::next_combination (first, middle, last, comp);
  }
  virtual bool prev ()
  {
    return boost::prev_combination (first, middle, last, comp);
  }
protected:
  virtual bool self_check () const
  {
    return (test_sequence_grow (first, middle, comp)
	    && test_sequence_grow (middle, last, comp));
  }
};

template <typename T>
struct DefaultIncrementor
{
  T& operator () (T& t)
  {
    return ++t;
  }
};

template <typename T>
struct DefaultDecrementor
{
  T& operator () (T& t)
  {
    return --t;
  }
};

template <typename Iter, 
	  typename T = typename std::iterator_traits<Iter>::value_type,
	  typename Incrementor = DefaultIncrementor<T>, 
	  typename Decrementor = DefaultDecrementor<T> >
class test_mapping: public test_base<Iter, std::less<T> >
{
  Iter first;
  Iter last;
  T first_value;
  T last_value;
  
  Incrementor inc;
  Decrementor dec;
public:
  test_mapping (Iter _first, Iter _last,
		T _first_value, T _last_value,
		Incrementor _inc = Incrementor(), 
		Decrementor _dec = Decrementor())
    : test_base<Iter, std::less<T> >(std::less<T>()),
      first(_first), last(_last), 
      first_value(_first_value), last_value(_last_value),
      inc(_inc), dec(_dec)
  {
  }
  
  virtual Iter begin() const
  {
    return first;
  }
  virtual Iter end() const
  {
    return last;
  }
  virtual Iter data_begin() const
  {
    return first;
  }
  virtual Iter data_end() const
  {
    return last;
  }

  virtual void init (bool min)
  {
    if (min)
      std::fill (first, last, first_value);
    else
      {
	T v = last_value;
	dec(v);
	std::fill (first, last, v);
      }
  }

  virtual bool next ()
  {
    return boost::next_mapping (first, last, first_value, last_value, inc);
  }

  virtual bool prev ()
  {
    return boost::prev_mapping (first, last, first_value, last_value, dec);
  }
protected:
  virtual bool self_check () const
  {
    return true;
  }
};

template <typename Iter, 
	  typename T = typename std::iterator_traits<Iter>::value_type,
	  typename Incrementor = DefaultIncrementor<T>, 
	  typename Decrementor = DefaultDecrementor<T> >
class test_combination_counts: public test_base<Iter, std::less<T> >
{
  Iter first;
  Iter last;
  T first_value;
  T last_value;

  Incrementor inc;
  Decrementor dec;

  std::vector<int> n;

  size_t number;
public:
  test_combination_counts(Iter _first, Iter _last,
			  T _first_value, T _last_value,
			  Incrementor _inc = Incrementor(),
			  Decrementor _dec = Decrementor())
    : test_base<Iter, std::less<T> >(std::less<T>()),
      first(_first), last(_last),
      first_value(_first_value), last_value(_last_value),
      inc(_inc), dec(_dec)
  {
    size_t count = 0;
    for (T t = _first_value; t != _last_value; _inc(t))
      ++count;

    n.resize(count);
    
    number = std::distance(_first, _last);
  }

  virtual Iter begin() const
  {
    return first;
  }
  virtual Iter end() const
  {
    return last;
  }
  virtual Iter data_begin() const
  {
    return begin();
  }
  virtual Iter data_end() const
  {
    return end();
  }
private:
  void transform ()
  {
    // how to fix this???

    sequence<std::vector<int>::iterator> seq(n.begin(), n.end());
    std::cout << "trans:";
    seq.out ();

    Iter iter = first;
    T v = first_value;
    for (std::vector<int>::iterator i = n.end(); i != n.begin();)
      {
	--i;
	for (int j = 0; (j < *i) && (iter != last); ++j)
	  *iter++ = v;
	inc (v);
      }

    sequence<Iter> seq_v(first, last);
    std::cout << "value:";
    seq_v.out ();
  }
public:
  virtual void init (bool min)
  {
    std::fill (n.begin(), n.end(), 0);
    if (min)
      n.back() = number;
    else
      n.front() = number;

    transform ();
  }
  virtual bool next ()
  {
    bool ret = boost::next_combination_counts (n.begin(), n.end());

    transform ();

    return ret;
  }
  virtual bool prev ()
  {
    bool ret = boost::prev_combination_counts (n.begin(), n.end());

    transform ();

    return ret;
  }
protected:
  virtual bool self_check () const
  {
    int num = 0;
    for (std::vector<int>::const_iterator i = n.begin(); i != n.end(); ++i)
      num += *i;
    return num == number;
  }
};

int main()
{
  int a[5] = {1, 2, 3, 4, 5};
  test_partial_permutation<int*> tpp(a, a + 3, a + 5);
  test_combination<int*> tc(a, a + 3, a + 5);
  test_mapping<int*> tm(a, a + 5, 1, 5);
  test_combination_counts<int*> tcc(a, a + 5, 1, 5);

  try
    {
      tpp.test ();
    }
  catch (const std::exception& e)
    {
      cout << "test_partial_permutation: " << e.what() << endl;
    }
  
  try
    {
      tc.test ();
    }
  catch (const std::exception& e)
    {
      cout << "test_combination: " << e.what() << endl;
    }

  try
    {
      tm.test ();
    }
  catch (const std::exception& e)
    {
      cout << "test_mapping: " << e.what() << endl;
    }

  try
    {
      tcc.test ();
    }
  catch (const std::exception& e)
    {
      cout << "test_combination_counts: " << e.what() << endl;
    }
}
