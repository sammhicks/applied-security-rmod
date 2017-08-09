#include "modmul.hpp"

void repeat_stage(void (*stage)()) {
  while (!cin.eof()) {
    stage();
  }
}

/*
Perform stage 1:

- read each 3-tuple of N, e and m from stdin,
- compute the RSA encryption c, then
- write the ciphertext c to stdout.
*/
void stage1() {
  BigInt N, e, m;

  cin >> N >> e >> m;

  if (!cin.eof()) {
    ModIntFactory f(N);

    ModInt m_mod = f.create_int(m);

    cout << static_cast<BigInt>(m_mod.pow(e)) << endl;
  }
}

/*
Perform stage 2:

- read each 9-tuple of N, d, p, q, d_p, d_q, i_p, i_q and c from stdin,
- compute the RSA decryption m, then
- write the plaintext m to stdout.
*/
void stage2() {}

/*
Perform stage 3:

- read each 5-tuple of p, q, g, h and m from stdin,
- compute the ElGamal encryption c = (c_1,c_2), then
- write the ciphertext c to stdout.
*/

void stage3() {}

/*
Perform stage 4:

- read each 5-tuple of p, q, g, x and c = (c_1,c_2) from stdin,
- compute the ElGamal decryption m, then
- write the plaintext m to stdout.
*/

void stage4() {}

int main(int argc, char *argv[]) {
  if (2 != argc) {
    abort();
  }

  if (!strcmp(argv[1], "stage1")) {
    repeat_stage(stage1);
  } else if (!strcmp(argv[1], "stage2")) {
    repeat_stage(stage2);
  } else if (!strcmp(argv[1], "stage3")) {
    repeat_stage(stage3);
  } else if (!strcmp(argv[1], "stage4")) {
    repeat_stage(stage4);
  } else {
    abort();
  }

  return EXIT_SUCCESS;
}
