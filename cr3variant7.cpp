#include <iostream>
#include <cmath>
#include <fstream>  
#include <string>
//here is ascii.txt programm


void cleanText(std::string pathText, std::string pathOut, int &CTlen)
{
        
        std::ofstream cleanCTstream(pathOut);
        std::ifstream dirtyCTstream(pathText);

        std::string str;
        char lastChar;
        while(std::getline(dirtyCTstream, str))
        {
            lastChar = str[str.length() - 1];
            str.erase(str.length()-1);
            CTlen += (str.length());
            cleanCTstream << str;
        }
        cleanCTstream << lastChar;
        CTlen++;

        cleanCTstream.close();
        dirtyCTstream.close();

}

        
void CTtoNum(std::string pathCT, int* CTnumeric)
{
        std::ifstream CTstream(pathCT);
        std::string str;
        std::getline(CTstream, str);

        CTstream.close();
       

        int numLet;
        for(int i = 0; i < str.length(); ++i )
        {
                numLet = str[i];
                numLet = 32 + numLet;// ascii is 8 bit code, ciryllic lower case is in range:224-255 (31letter) 
                                      //but in binary this range consists of 'negative' numbers, so i do a = 224 - (-a(mod256))
                                      //so i get 'a' = 0; 'б' = 1 ... 'я' = 31
                //but ascii table isn't good for lab3
                //a = 0, б = 1, ..., щ = 25, ь = 26, ы = 27, э = 28, ю = 29, я =30 
                if(numLet == 28 )//deal with ь
                {
                        numLet = 26;
                }
                if(numLet > 27)
                {
                        numLet = numLet - 1;
                }

               

                CTnumeric[i] = numLet;

        }

}

void mfbCTfind(int* CTnumeric, int CTlen, int *mfbCT)
{
  int bigrCT[961] = {};

  for(int i = 0; i < CTlen; ++i)
  {
      bigrCT[((CTnumeric[i] * 31) + CTnumeric[i+1] )] += 1;
      i++;
  }
  

  int amount, position; //amount - how many such bigramms in CT; position - number of tis bigramm
  for(int i = 0; i < 5; ++i)
  {
    for(int j= 0; j < 961; ++j)
    {
        if(bigrCT[j] > amount)
        {
                amount = bigrCT[j];
                position = j;
        }
    }

    mfbCT[i] = position;
    bigrCT[position] = -1;
    amount = position = 0;
  }

}


int extendedEucAlg(int a, int b, int &u, int &v)//"handbook of applied cryptogrphy" (page 67)
        //unsigned
{   
     if(b == 0)
     {
         u = 1; 
         v = 0; 
       return a;
    
     }
     int u1 = 0, u2 = 1, v1 = 1, v2 = 0;
     int q, r;
     while (b > 0)
     {
           q = a/b;
           r = a - q*b;
           u = u2 - q*u1;
           v = v2 - q*v1;
           a = b;
           b = r;
           u2 = u1;
           u1 = u;
           v2 = v1;
           v1 = v;
       
      }
        u = u2;
        v = v2;
       return a;
}

int linComp(int a, int b, int mod, int &extraSolutions, int& u, int &v)
        //unsigned
{
  int x, gcd;

  gcd = extendedEucAlg(a, mod, u, v);
  u < 0 ? u = mod + u : u = u;//if the value of u,v is negative - here we find it's positive form 
  v < 0 ? v = mod + v : v = v;
  if(gcd == 1)
  {
      return (((a*u) % mod) == 1) ? (x = (u*b) % mod) : (x = (v*b) % mod); 
  }
  else
  {
    if((b % gcd) != 0)
    {
            return x = 1000; // false state: comparison has no solutions
    }
    else
    {
            extraSolutions = gcd;
            return x = linComp((a/gcd), (b/gcd), (mod/gcd), extraSolutions, u, v);
    }
  }

}

int aKey(int deltaX, int deltaY, int &u, int &v, int& extS)
       //unsigned
{
  return linComp(deltaX, deltaY, 961, extS, u, v);
}

int bKey(int a, int X1, int Y1)
        //unsigned
{
  return (Y1 - X1*a) < 0 ? 961 + ((Y1 - X1*a) % 961) : ((Y1 - X1*a) % 961);
}

int negative(int a, int mod)//a = -a(mod n)
{
      return (a < 0) ? a = (mod + a) % mod : a = (mod - a) % mod;
}

int opposite(int a, int mod)//a = (a)^-1 (mod n)
{ 
        int u, v, opos;
        opos = extendedEucAlg(a, mod, u, v);
        u < 0 ? u += mod : u = u;
        v < 0 ? v += mod : u = u;

        return(((a*u)% mod) == 1) ? a = u : a = v;
}

void decrypt(int a, int b, std::string pathPTone,std::string pathPTall , int* CTnumeric, int CTlen, int* frequency)//here a  = (a)^-1; b = -b(mod961)
        //TODO TODO TODO TODO TODO
{
    std::ofstream onePTstream(pathPTone);
    std::ofstream allPTstream(pathPTall, std::ios::app);


    for(int i = 0 ; i < 31; ++i)
    {
            frequency[i] = 0;
    }

    onePTstream << '\n' << "a = " << a << ", b = " << b << '\n';
    allPTstream << '\n' << "a = " << a << ", b = " << b << '\n';

    a = opposite(a, 961);
    b = negative(b, 961);

    char PT[CTlen] = {};
    int bigr, letter;
    for(int i = 0; i < CTlen; ++i)
    {
      bigr = CTnumeric[i]*31 + CTnumeric[i+1];
      bigr = (a*(bigr + b)) % 961;
      
      //1st letter
      letter = bigr / 31;
      frequency[letter] +=1;
      //here we invert lab's table to ascii
      if(letter > 27) ++letter;
      if(letter == 26) letter = 28;
      letter = -32 + letter;
      PT[i] = letter;
      
      //second letter
      ++i;
      letter = bigr % 31;
      frequency[letter] +=1;
      if(letter > 27) ++letter;
      if(letter == 26) letter = 28;
      letter = -32 + letter;
     PT[i] = letter;
     }
    
    onePTstream << PT << '\n' << '\n'<< '\n';
    allPTstream << PT << '\n' << '\n'<< '\n';


    onePTstream.close();
    allPTstream.close();
    
}



float  checkEnt(int CTlen, int* freq)
{
  float ent = 0.0;

  for(int i = 0; i < 31; ++i)
  {
          if(freq[i] == 0)
          {
                  continue;
          }
          else
          {
                ent += -((freq[i]/((float)CTlen))*(log2(freq[i] / ((float)CTlen))));
          }
  }
 return ent;
}

void leGrandeFinale(int CTlen, int* CTnumeric, int* frequency, std::string pathPTall, std::string pathPTone, const int* mfbCT, const int* mfbPT)
{
  int a, b, extS, deltaX, deltaY, u, v;
  int aMem;
  float entr;
       
  for(int Y1 = 0; Y1 < 5; ++Y1)
        {
                for(int Y2 = 0; Y2 < 5; ++Y2)
                {
                        for(int X1 = 0; X1 < 5; ++X1)
                        {
                                for(int X2 = 0; X2 < 5; ++X2)
                                {
                                    if(Y1 == Y2) break; 
                                    if(X1 == X2) break;
                                      
                                    extS = 0;
                                    (mfbCT[Y1] >= mfbCT[Y2]) ? deltaY = mfbCT[Y1] - mfbCT[Y2] : deltaY = 961 + (mfbCT[Y1] - mfbCT[Y2]);
                                    (mfbPT[X1] >= mfbPT[X2]) ? deltaX = mfbPT[X1] - mfbPT[X2] : deltaX = 961 + (mfbPT[X1] - mfbPT[X2]);

                                    a = aKey(deltaX, deltaY, u, v,extS );

                                    if( a != 1000)
                                    {
                                      aMem = a;
                                      if(extS == 0)
                                      {
                                              b = bKey(a, mfbPT[X1], mfbCT[Y1]);
                                             std::cout <<  "a = " << a << "b = " << b << std::endl;
                                                      decrypt(a, b, pathPTone, pathPTall, CTnumeric, CTlen, frequency);
                                                      entr = checkEnt(CTlen, frequency);
                                                     if(entr> 4 && entr < 4.5)
                                                     {
                                                             std::cout << "BINGO" << std::endl;
                                                             return;
                                                     }
                                      }                      
                                      else
                                      {
                                              for(int i = 0; i < extS; ++i)
                                              {
                                                      a = aMem + (i*(961/extS)); 
                                                      b = bKey(a, mfbPT[X1], mfbCT[Y1]);
                                                      std::cout << "a =" << a << " aMem =" << aMem << " b=" << b << std::endl;
                                                      decrypt(a, b, pathPTone, pathPTall, CTnumeric, CTlen, frequency);
                                                      entr = checkEnt(CTlen, frequency);
                                                     if(entr > 4 && entr < 4.5)
                                                     {
                                                             std::cout << "BINGO" << std::endl;
                                                             return;
                                                     } 
                                              }

                                      }

                                    }
                                    else
                                    {
                                      std::cout << "NS" << std::endl;
                                    } 
                                    extS = 0;
                                }
                        }
                }
        }



}




int main()
{
        std::string pathCT = "/home/ftl/2019cr3/code/ascii.txt";
        std::string pathCTclean = "/home/ftl/2019cr3/code/outF.txt";
        std::string pathPTone = "/home/ftl/2019cr3/code/answer(onePT).txt";
        std::string pathPTall = "/home/ftl/2019cr3/code/all(allPT).txt";

        std::ofstream clear(pathPTall);
        clear << ' ';
        clear.close();


        int CTlen = 0;        
        const int mfbPT[5] = {545, 417, 572, 403, 168};


        cleanText(pathCT, pathCTclean, CTlen);
        int CTnumeric[CTlen] = {};
        int OTnumeric[CTlen] = {};
        CTtoNum(pathCTclean, CTnumeric); 

        int mfbCT[5] = {};
        
        mfbCTfind(CTnumeric, CTlen, mfbCT);
        int frequency[31] = {};
        
        leGrandeFinale(CTlen, CTnumeric, frequency, pathPTall, pathPTone, mfbCT, mfbPT);
  return 0;
}
