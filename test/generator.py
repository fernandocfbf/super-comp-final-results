import random

i=2

index=0
n_size = ""
while i < 1000:
     n = i # tamanho da primeira sequência
     m = i # tamanho da segunda sequência
     file = "input{0}".format(index) # nome do arquivo a ser gerado
     f = open(file, 'w')
     seq=[str(n)+'\n',
          str(m)+'\n',
          ''.join(random.choices(['A','T','C','G','-'],k=n))+'\n',
          ''.join(random.choices(['A','T','C','G','-'],k=m))]
     f.writelines(seq)
     f.close()
     print(''.join(seq))
     n_size += " {0}".format(i)
     i += 1
     index+=1
f = open("n-sizes", 'w')
f.writelines(str(n_size))
f.close()
