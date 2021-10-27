#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

void computavoto(int voto,vector<pair<int,int>> &cargo)
{
  if(cargo[voto].second!= voto)
    cargo[voto]=make_pair(1,voto);
  else
    cargo[voto].first++;
}
int main(int argc, char **argv)
{
  int S,F,E;
  int voto;
  int validos=0, invalidos=0;
  int validospres=0;
  if(argc==2)
    freopen(argv[1],"r",stdin);
  //map<int,int> presidente,senador,depfed,depest;
  vector<pair<int,int>> presidente(100),senador(1000),depfed(10000),depest(100000);
  scanf("%d %d %d%*c",&S,&F,&E);
#ifdef INGENUO
  while(scanf("%d",&voto)==1)
  {
#else
  while(1)
  {
    int tmp=getchar_unlocked();
    if(tmp==-1) break;
    int neg=0;
    voto=0;
    while(tmp!='\n')
    {
      if(tmp=='-') neg=1;
      else
        voto=voto*10+(tmp-'0');
      tmp=getchar_unlocked();
    }
    if(neg) voto*=-1;
#endif
    if(voto<0)
    {
      invalidos++;
      //printf("======== %d\n",voto);
      continue;
    }

    validos++;
    if(voto<100)
    {
      computavoto(voto,presidente);
      validospres++;
    }
    else if(voto<1000)
      computavoto(voto,senador);
    else if(voto<10000)
      computavoto(voto,depfed);
    else
      computavoto(voto,depest);
  }
  printf("%d %d\n",validos,invalidos);
  int presvence=validospres*51/100;
  if( validospres*51%100>0)
    presvence++;

  int pv=0;
  for(auto pres: presidente)
  {
    if(pres.first >= presvence)
    {
      printf("%d\n",pres.second);
      pv=1;
      break;
    }
  }
  if(pv!=1)
    printf("Segundo turno\n");

  sort(senador.begin(),senador.end(), std::greater<>());
  sort(depfed.begin(),depfed.end(), std::greater<>());
  sort(depest.begin(),depest.end(), std::greater<>());

  printf("%d",senador[0].second);
  for(int i=1;i< S;i++)
    printf(" %d",senador[i].second);

  printf("\n");
  printf("%d",depfed[0].second);
  for(int i=1;i< F;i++)
    printf(" %d",depfed[i].second);
  printf("\n");
  printf("%d",depest[0].second);
  for(int i=1;i< E;i++)
    printf(" %d",depest[i].second);
  printf("\n");
}
