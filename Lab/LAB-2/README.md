### Important System calls used
`readv()`
`writev()`

### Assumptions made
i'th buffer of iovcnt is of size 8 * ` sizeof(char) `, where iovcnt=2\
Hence the chunks read and written are 8 * ` sizeof(char) `\
Max Size transfer per transaction is 2 * 8 * ` sizeof(char) `\
Input must be 7char long for correct functioning due to termination with ` \0 `
