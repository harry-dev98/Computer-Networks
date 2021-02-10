### System calls used
`readv()`
`writev()`

### Assumptions made
ith buffer of iovcnt is of size 8 * ` sizeof(char) `, where iovcnt=2__
Hence the chunks read and written are 8 * ` sizeof(char) `__
Max Size transfer per transaction is 2 * 8 * ` sizeof(char) `__
Input must be 7char long for correct functioning due to termination with ` \0 `