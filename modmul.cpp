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
    ModIntFactory N_f(N);

    ModInt m_mod_N = m % N_f;

    ModInt c_mod_N = m_mod_N.pow(e);

    BigInt c = static_cast<BigInt>(c_mod_N);

    cout << c << endl;
  }
}

/*
Perform stage 2:

- read each 9-tuple of N, d, p, q, d_p, d_q, i_p, i_q and c from stdin,
- compute the RSA decryption m, then
- write the plaintext m to stdout.
*/
void stage2() {
  BigInt N, d, p, q, d_p, d_q, i_p, i_q, c;
  cin >> N >> d >> p >> q >> d_p >> d_q >> i_p >> i_q >> c;

  if (!cin.eof()) {
    ModIntFactory p_f(p), q_f(q), N_f(N);

    // m = c^d mod N, but using CRT

    // m1 = c^d_p mod p
    ModInt m1_mod_p = (c % p_f).pow(d_p);
    // m2 = c^d_q mod q
    ModInt m2_mod_q = (c % q_f).pow(d_q);

    // m_diff = (m1 - m2) mod p
    // This accounts for the case when m2 > m1, and seeing as we later on mod by
    // p anyway, we don't lose anything
    ModInt m_diff_mod_p = m1_mod_p;
    m_diff_mod_p -= m2_mod_q % p_f;

    // h = i_q(m1 - m2) mod p
    ModInt h_mod_p = (i_q % p_f) * m_diff_mod_p;

    // m = (m2 + h * q) mod N
    ModInt m_mod_N = m2_mod_q % N_f;
    m_mod_N += (h_mod_p % N_f) * (q % N_f);

    BigInt m = static_cast<BigInt>(m_mod_N);

    cout << m << endl;
  }
}

/*
Perform stage 3:

- read each 5-tuple of p, q, g, h and m from stdin,
- compute the ElGamal encryption c = (c_1,c_2), then
- write the ciphertext c to stdout.
*/

void stage3() {
  BigInt p, q, g, h, m;

  cin >> p >> q >> g >> h >> m;

  if (!cin.eof()) {
    ModIntFactory p_f(p);

    BigInt k(1);

    // c1 = g^k mod p
    ModInt c1_mod_p = (g % p_f).pow(k);

    // s = h^k mod p
    ModInt s_mod_p = (h % p_f).pow(k);

    // c2 = (m * s) % p;
    ModInt c2_mod_p = (m % p_f) * s_mod_p;

    BigInt c1 = static_cast<BigInt>(c1_mod_p);
    BigInt c2 = static_cast<BigInt>(c2_mod_p);

    cout << c1 << endl << c2 << endl;
  }
}

/*
Perform stage 4:

- read each 5-tuple of p, q, g, x and c = (c_1,c_2) from stdin,
- compute the ElGamal decryption m, then
- write the plaintext m to stdout.
*/

void stage4() {
  BigInt p, q, g, x, c1, c2;

  cin >> p >> q >> g >> x >> c1 >> c2;

  if (!cin.eof()) {
    ModIntFactory p_f(p);

    // m = c2*(c1 ^ (q-x)) mod p
    ModInt m_mod_p = (c2 % p_f) * (c1 % p_f).pow(q - x);

    BigInt m = static_cast<BigInt>(m_mod_p);

    cout << m << endl;
  }
}

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
