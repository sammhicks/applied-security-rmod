
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"
#include "modint.hpp"

//#include "list.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {

  BigInt N("91D367C0E4B3B770582CB42ADC85D5223C77848ED52CD5A9F811F879EE609B1016E"
           "8D99579FD367A6257E6FC4624C8C542BB6B8E637DA7271B51E4D6A07C8599997739"
           "37BE9799F527EA5339AE3E2FD506B2885997E7BE28484109B7965E41CACC2DE8434"
           "32AFA0304A5C7345BF8BDAAB8A016FDF2E01D0DCC2C65B478A16AFD");
  BigInt e("2000773B284CEEDF06B0708E20116296564F3480DF5B3A4B044EF2737C0EE4CA2D3"
           "DBF7D57F9233F6718FEF948CA580B20DD95313D14999688CC3FDF75B36C6F45E7F9"
           "38CDE5F2B3D16EA35062BD39D4D3FA8939B21CB3784A3130DF25F401E2DC8548E51"
           "9A0788707E7CEB4B424E8AC46F3C50F2810FD80959C068E6EDD7BE7");
  BigInt m("244E30656CED97EC5465392B970C14CB1264ED8D46575A91C663C2032A5F5F6C570"
           "8AE03C21E45B8C866DB18B9BF1737A455E0176262A9177F64A5B6F8F0ED695DE659"
           "45617189AF82B5BA757F05011FA14979B70CBD848F568EE915F2FD700B6208A6159"
           "D6BADF40B98E05427CA0A6D35BA1C4C338C1F1CE25879CE510F874C");

  /*ModIntFactory f(N);

  ModInt m_mod = f.create_int(m);

  cout << "Ready? Here goes..." << endl;

  cout << static_cast<BigInt>(m_mod.pow(e)) << endl;*/

  cout << "Creating Factory" << endl;

  ModIntFactory f(N);

  cout << "Creating m_mod" << endl;
  ModInt m_mod = f.create_int(m);

  cout << "Creating e_mod" << endl;
  ModInt e_mod = f.create_int(m);

  cout << "Multiplying..." << endl;

  ModInt me_mod = m_mod * e_mod;

  cout << "Loop..." << endl;

  while (true) {
    cout << static_cast<BigInt>(me_mod) << endl;

    me_mod = me_mod * me_mod;
  }

  return EXIT_SUCCESS;
}
