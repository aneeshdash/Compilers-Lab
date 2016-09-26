int fact;
int factorial (int n)
{
	if (n == 1)
    	fact = fact * 1;
    else
    	fact = n * factorial(n-1);
    return;
}

bool checkPerfectSquare (int a)
{
	bool res;
    res = false;
    int i;
    i = 1;
    while (i < a)
    {
        while(i<2)
        {
            i = i;
            print i;
        }
    	if (i*i == a)
        	res = true;
        i = i + 1;
    }
}
    return res;
}

int main()
{
	int n;
    read n;
    if (checkPerfectSquare(n) == true)
    	print 1;
    else
    {
    	factorial(n);
        print fact;
    }
    return;
}