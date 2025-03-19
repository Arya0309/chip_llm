// C++ program to remove leading zeros 
// from a given string 
#include <iostream> 

using namespace std; 

string removeZero(string str) 
{ 
	// Count leading zeros 
	int i = 0; 
	while (str[i] == '0') 
		i++; 

	// The erase function removes i characters 
	// from given index (0 here) 
	str.erase(0, i); 

	return str; 
} 

// Driver code 
int main() 
{ 
	string str; 
	str = "00000123569"; 
	str = removeZero(str); 
	cout << str << endl; 
	return 0; 
}
