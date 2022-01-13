# Final Assingment Computer Stractures BIU.

That is the final assingment of Computer Stractures course at Bar Ilan University.

## Assingment Explanation:
Optimize the code inside myfunc.c to achive lowest running time achivable.

Full explenation of the assingment is provided here (hebrew source):
```bash
wget https://github.com/Etelis/CS_EX5/blob/master/Ex5.pdf
```

## Usage

You  could use compare.sh to run the program and check if any diffrances were found between provided files and program created:

```bash
./compare.sh 20
```
Note: checker was note made by me.

## Algoritm  and implementation 
 The algorithm used is quite simple, let i be the current row of the iteration process, if colum 0 + colum 1 are summed in the outer - loop the inner loop could only sum the third row to achieve the sum of the cube (3x3).
after doing so moving to j++, actually colum0 = colum 1, colum 1 = colum 2 by understanding so we could decrease number of calculations done dramatically.

## Score time
higher time score on planet server was 7ms non filtered 10ms filtered.
