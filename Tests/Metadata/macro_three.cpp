
#ifdef DEFINE_macro_three
int macro_three()
{
  return 42;
}
#endif

int main()
{
  return macro_three() - 42;
}
