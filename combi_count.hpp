template <typename BiIter>
bool
next_repeat_combination(BiIter first, BiIter last)
{
	typedef typename std::iterator_traits<BiIter>::value_type value_type;
	if (first == last)
		return false;

	BiIter tail = last;
	if (--tail == first)
		return false;
	value_type tv = *tail;
	*tail = 0;

	BiIter end = tail;
	while (end != first)
	{
		BiIter fe = end;
		if (*(--fe) == 0)
			end = fe;
		else
			break;
	}
	if (end == first)
	{
		*end = *end + tv;
		return false;
	}

	BiIter dec = end;
	--*--dec;
	*end = tv + 1;
	return true;
}

template <class BiIter>
bool
prev_repeat_combination(BiIter first, BiIter last)
{
	typedef typename std::iterator_traits<BiIter>::value_type value_type;
	if (first == last)
		return false;

	BiIter tail = last;
	if (--tail == first)
		return false;

	BiIter end = last;
	while (end != first)
	{
		BiIter fe = end;
		if (*(--fe) == 0)
			end = fe;
		else
			break;
	}

	if (end != first)
		--end;
	value_type tv = *end;
	*end = 0;
	if (end != first)
	{
		BiIter fe = end;
		++*--fe;
		--tv;
	}

	*tail = tv;

	return end != first;
}
