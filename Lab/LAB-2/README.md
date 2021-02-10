### System calls used
`readv()`
`writev()`

### Assumptions made
ith buffer of iovcnt is of size 8 * ` sizeof(char) `, where iovcnt=2
Hence the chunks read and written are 8 * ` sizeof(char) `
Max Size transfer per transaction is 2 * 8 * ` sizeof(char) `