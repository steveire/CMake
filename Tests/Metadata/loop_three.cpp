
#ifdef DEFINE_loop_three
int loop_three()
{
  return 42;
}
#endif

int main()
{
  return loop_three() - 42;
}
