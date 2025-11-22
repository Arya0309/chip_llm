#include <iostream> 
#include <fstream>
using namespace std; 

int binaryToDecimal(int n) 
{ 
	int num = n; 
	int dec_value = 0; 

	int base = 1; 

	int temp = num; 
	while (temp) { 
		int last_digit = temp % 10; 
		temp = temp / 10; 
		dec_value += last_digit * base; 
		base = base * 2; 
	} 

	return dec_value; 
} 

int main() 
{ 
	ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");
	int num; 
	int count = 0;
	while (count < 100 && infile >> num) { 
        outfile << binaryToDecimal(num) << endl; 
        count++;
    } 
    infile.close();
    outfile.close();
}
