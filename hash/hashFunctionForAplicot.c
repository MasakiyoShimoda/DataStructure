//hashing and finding alpha
//2017.6.26
//N.SUN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define H 47
#define K 60

typedef unsigned int INDEX;


//new hash function created by yourself
INDEX hash5(int H_SIZE, int seed, int i){
    if(i == 0)
        srand(seed);
    return (rand() % H_SIZE);
}

void getStrings(char a[H][K]){
    FILE *fp;
    int i;
    
    fp=fopen("2017HI4.txt","r");
    for(i=0; i<H; i++)
        fscanf(fp,"%s",a[i]);
    fclose(fp);
}

int main(){
    char list[H][K]; //namelist
    int HashList[H] = {0}; //list of hash values for the above namelist
    int i,x,count=0;
    
    getStrings(list);
    for(i=0; i<H; i++){
        x=hash5(H, 2059529867, i);
        HashList[x]++;
        
        //histogram
        
        printf("%d\t : %s\n",x,list[i]);
    }
    
    printf("\n\n%s\t : %s\n","index ","the number of hashed items");
    for(i=0; i< H; i++){
        printf("%d\t : %d \n", i, HashList[i]);
        
        if(HashList[i])
            count++;
    }
    
    printf("\nThe number of hashed items: %d \n", count);
    printf("\nalpha = %lf \n\n", (double)(count)/H);
}
