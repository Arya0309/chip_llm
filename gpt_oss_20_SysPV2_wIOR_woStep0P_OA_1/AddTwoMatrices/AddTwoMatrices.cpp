/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    two 4×4 matrices
Expected output consists of:
    a 4×4 matrix
*/
using namespace std;
#define N 4

void add(int A[][N], int B[][N], int C[][N])
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            C[i][j] = A[i][j] + B[i][j];
}