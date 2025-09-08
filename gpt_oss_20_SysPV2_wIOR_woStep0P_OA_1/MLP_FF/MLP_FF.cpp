/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
    input: a 32-element vector of float
    w1: a 16×32 matrix of float
    b1: a 16-element vector of float
    w2: a 10×16 matrix of float
    b2: a 10-element vector of float
Expected output consists of:
    output: a 10-element vector of float
*/

const int INPUT_SIZE  = 32;
const int HIDDEN_SIZE = 16;
const int OUTPUT_SIZE = 10;

void mlp_feedforward(
    const float input[INPUT_SIZE],
    float hidden[HIDDEN_SIZE],
    float output[OUTPUT_SIZE],
    const float w1[HIDDEN_SIZE][INPUT_SIZE],
    const float b1[HIDDEN_SIZE],
    const float w2[OUTPUT_SIZE][HIDDEN_SIZE],
    const float b2[OUTPUT_SIZE]
) {
    for (int i = 0; i < HIDDEN_SIZE; ++i) {
        float sum = b1[i];
        for (int j = 0; j < INPUT_SIZE; ++j) {
            sum += w1[i][j] * input[j];
        }
        hidden[i] = (sum > 0.0f) ? sum : 0.0f;  // ReLU activation
    }
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        float sum = b2[i];
        for (int j = 0; j < HIDDEN_SIZE; ++j) {
            sum += w2[i][j] * hidden[j];
        }
        output[i] = sum;
    }
}