#include <bits/stdc++.h>
using namespace std;

#define R1 2
#define C1 2
#define R2 2
#define C2 3

void mulMat(int mat1[][C1], int mat2[][C2])
{
	int rslt[R1][C2];

	cout << "Multiplication of given two matrices is:\n";

	for (int i = 0; i < R1; i++) {
		for (int j = 0; j < C2; j++) {
			rslt[i][j] = 0;

			for (int k = 0; k < R2; k++) {
				rslt[i][j] += mat1[i][k] * mat2[k][j];
			}

			cout << rslt[i][j] << "\t";
		}

		cout << endl;
	}
}

int main()
{
	int mat1[R1][C1] = { { 1, 1 }, { 2, 2 } };

	int mat2[R2][C2] = { { 1, 1, 1 }, { 2, 2, 2 } };

	if (C1 != R2) {
		cout << "The number of columns in Matrix-1 must "
				"be equal to the number of rows in "
				"Matrix-2"
			<< endl;
		cout << "Please update MACROs according to your "
				"array dimension in #define section"
			<< endl;

		exit(EXIT_FAILURE);
	}

	mulMat(mat1, mat2);

	return 0;
}
