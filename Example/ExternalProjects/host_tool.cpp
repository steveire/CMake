
#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
  std::ofstream myfile;
  myfile.open (argv[1]);
  myfile << "#define " << argv[2] << "\n";
  myfile.close();
  return 0;
}
