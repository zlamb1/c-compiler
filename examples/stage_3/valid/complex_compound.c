int main()
{
    int a = 0;
    int b = (a += 3) * 3;
    return a + b;  
}