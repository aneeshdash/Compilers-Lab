static int fact = 1;

int factorial(int n)
{
	if (n == 1)
    	fact = fact * 1;
    else
    	fact = n * factorial(n-1);
    return;
}

bool checkPerfectSquare (int a)
{
	bool res = false;
    int i = 2;
    while (i < a)
    {
    	if (i*i == a)
        	res = true;
        i = i + 1;
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