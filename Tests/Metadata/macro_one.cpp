
#ifdef DEFINE_macro_one
int macro_one()
{
  return 42;
}
#endif

int main()
{
  return macro_one() - 42;
}
