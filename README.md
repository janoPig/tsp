# tsp

Combined hill-climbing search with path transformation. 

```
input = random_path()
it = 0
do
  it++
  
  do_mutation(input)
  path = transform(input)
  length = evaluate_path(path)
  
  if (length > bestLength):
    undo_mutation(input)
  if (length < bestLength):
    bestLength = length
while it < limit

return transform(input)
```
