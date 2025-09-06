// C++ program to find Armstrong numbers between 1 to 1000 using a brute force approach
#include <bits/stdc++.h>
using namespace std;

// Function to return the order of a number.
int order(int num)
{
	int count = 0;
	while (num > 0)
	{
		num /= 10;
		count++;
	}
	return count;
}

// Function to check whether thegiven number is Armstrong numberor not
bool isArmstrong(int num)
{
	int order_n = order(num);
	int num_temp = num, sum = 0;

	while (num_temp > 0)
	{
		int curr = num_temp % 10;
		sum += pow(curr, order_n);
		num_temp /= 10;
	}
	if (sum == num)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Driver code
int main()
{

	cout << "Armstrong numbers between 1 to 1000 : ";
	// Loop which will run from 1 to 1000
	for (int num = 1; num <= 1000; ++num)
	{

		if (isArmstrong(num))
		{
			cout << num << " ";
		}
	}
	return 0;
}
