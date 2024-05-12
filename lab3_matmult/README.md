# Matrix multiplicatoin using MPI

## Result

In `res` folder.

## Usage
```bash
mpirun -np 4 ./matmult data.mat result.mat
```
## File format
### Input file
```
<rows in first matrix> <columns in first and first in second matrix> <rows in first matrix>
<first matrix>
<second matrix>
```
Example:
```
2 3 4
1 2 3
4 5 6
1 2 3 4
5 6 7 8
9 10 11 12
```

### Output file
```
<row> <columns>
<matrix>
```