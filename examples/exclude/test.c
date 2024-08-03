int main() {
    int i = 0, j = 0, k = 0, a = 0, n = 13;
    while (i < n)
    {
        if (i < 2)
        {
            j = 1; 
            k = 0;
        }
        a = j + k;
        k = j;
        j = a; 
        i++;
    }
    return a;
}