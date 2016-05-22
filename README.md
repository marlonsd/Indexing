# Indexing

Para executar, necessário instalar htmlcxx [http://htmlcxx.sourceforge.net].

No terminal, criar pastas:
  $ make dir
  
Compilar código, no terminal:
  $ make
  
Antes de executar:
  ulimit -n 4000 # Para mudar valor, deve-se mudar no código. Maiores detalhes no relatório
  export LD_LIBRARY_PATH="/usr/local/lib"
  
Para executar indexação, htmls devem estar na pasta htmls:
  ./indexing
  
Para busca binária:
  ./search
