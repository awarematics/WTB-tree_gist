







WTB_KEY_IN_NODE
range_key_to_node_key(wkey *w)
{
	WTB_KEY_IN_NODE n;
	
	if (w[0]=='l') // Leaf Node
	{
		for (int i=0; i<12; i++)
		{
			n->nkey[i] = w[i+1];
		}
	}
	
	return n;
}

WTB_KEY_IN_RANGE
node_key_to_range_key(wkey *w)
{
	WTB_KEY_IN_RANGE r;
	
	if (w[0]=='i') // Intermediate Node
	{
		for (int i=0; i<12; i++)
		{
			r->lkey[i] = w[i+1];
			r->ukey[i] = w[i+13];
		}
	}
	
	return r;
}

