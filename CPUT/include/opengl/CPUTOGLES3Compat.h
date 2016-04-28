//
// Use this file to auto create an ES3 compat layer
// This will be used to generate a list of function pointers
// to the ES3 functions, pulled from Intels ES2 library.
//
// Add to this file as needed

ES3_FUNC(void, glGenVertexArrays, (GLsizei n, GLuint* arrays))
ES3_FUNC(void, glDeleteVertexArrays, (GLsizei n, const GLuint* arrays))
ES3_FUNC(void, glBindVertexArray, (GLuint array))
ES3_FUNC(void, glVertexAttribIPointer, (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer))
ES3_FUNC(void, glBindSampler, (GLuint unit, GLuint sampler))
ES3_FUNC(void, glUniformBlockBinding, (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding))
ES3_FUNC(GLuint, glGetUniformBlockIndex, (GLuint program, const GLchar* uniformBlockName))
ES3_FUNC(void, glGetActiveUniformBlockName, (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName))
ES3_FUNC(void, glBindBufferBase, (GLenum target, GLuint index, GLuint buffer))
ES3_FUNC(void, glGenSamplers, (GLsizei count, GLuint* samplers))
ES3_FUNC(void, glSamplerParameteri, (GLuint sampler, GLenum pname, GLint param))
ES3_FUNC(void, glSamplerParameterf, (GLuint sampler, GLenum pname, GLfloat param))
