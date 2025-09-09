/* === SYNTHESIZABLE ROTATE MATRIX (4×4) ===
   This function rotates a 4×4 matrix by 90 degrees clockwise in place.
   No I/O operations are performed; the function is fully synthesizable.
*/
#define R 4
#define C 4

void rotatematrix(int mat[R][C])
{
    // Number of layers is floor(min(R, C)/2)
    const int layers = (R < C ? R : C) / 2;

    for (int layer = 0; layer < layers; ++layer)
    {
        int first = layer;
        int last  = (R - 1) - layer; // Since R == C == 4, last = 3 - layer

        for (int i = first; i < last; ++i)
        {
            int offset = i - first;

            // Save top
            int top = mat[first][i];

            // left -> top
            mat[first][i] = mat[last - offset][first];

            // bottom -> left
            mat[last - offset][first] = mat[last][last - offset];

            // right -> bottom
            mat[last][last - offset] = mat[i][last];

            // top -> right
            mat[i][last] = top;
        }
    }
}