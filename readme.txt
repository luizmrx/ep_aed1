Esse projeto foi desenvolvido utilizando o sistema operacional Linux, distro Mint. Para o funcionamento adequado do programa, foi necessário instalar as bibliotecas unac e uft8proc. Para fazer a instalação dessas bibliotecas, digite os seguintes comandos no terminal:

sudo apt-get install libunac1-dev
sudo apt-get install libutf8proc-dev

A biblioteca unac é responsável por remover os acentos e caracteres especiais de strings, enquanto a biblioteca uft8proc permite a padronização das palavras, transformando todas em minúsculas, por exemplo.

Antes de tentar executar o programa, deve-se compilá-lo com o seguinte comando:
gcc -o main main.c -lunac
./main texto.txt [tipo_de_busca]

[tipo_de_busca] deve ser substutuído por lista, caso deseja realizar a busca através de uma lista sequencial, ou por arvore, caso queira utilizar uma árvore AVL. Note que ao compilar o programa, devemos passar o parâmetro -lunac para que não ocorra nenhum erro de leitura.

O arquivo texto.txt é o arquivo que será carregado e analisado pelo programa. Ele deverá estar no mesmo diretório em que o main.c. Caso queira realizar outros testes com textos diferentes, favor editá-lo e salvá-lo antes de fazer a compilação.
