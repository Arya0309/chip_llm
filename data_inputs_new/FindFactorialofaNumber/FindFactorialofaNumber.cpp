#include <iostream>
#include <fstream>
using namespace std;

long long int factorial(long long int n)
{
	long long int res = 1, i;
	for (i = 2; i <= n; i++)
		res *= i;
	return res;
}

int main()
{
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

	long long int num;
    int count = 0;
	
	while (count < 100 && infile >> num)
    {
        // 輸出純數值結果
        outfile << factorial(num) << endl;
        count++;
    }

    infile.close();
    outfile.close();
	return 0;
}
