/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    a 4×4 matrix 
Expected output consists of:
    a 4×4 matrix
*/
#define n 4 

void interchangeFirstLast(int m[][n]) 
{ 
    for (int i = 0; i < n; i++) 
    { 
        int t = m[i][0]; 
        m[i][0] = m[i][n - 1]; 
        m[i][n - 1] = t; 
    } 
}