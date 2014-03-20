

inline
GLVertexArrayObject::GLVertexArrayObject(GLuint programID)
    : programID_(programID),
      primitiveType(GL_TRIANGLES), 
      numElements(0),
      numIndices(0)
{
    glGenVertexArrays(1, &vaoID_);
}

inline
GLVertexArrayObject::~GLVertexArrayObject()
{
    glBindVertexArray(vaoID_);
    for (auto& bufName : bufferNames_)
    {
        GLuint bufIndex = bufName.second;
        glDeleteBuffers(1, &bufIndex);
    }

    GLint buffer(0);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer);	
    if (buffer)
    {
        GLuint bufferIndex[1] = {static_cast<GLuint>(buffer)};
        glDeleteBuffers(1, bufferIndex);
    }

    glDeleteVertexArrays(1, &vaoID_);
}

inline
GLuint GLVertexArrayObject::vaoID() 
{ return vaoID_; }



template <typename T>
inline
void GLVertexArrayObject::attributedBuffer(const T &data, std::string attribute)
{
    glBindVertexArray(vaoID_);
    numElements = (GLsizei)data.cols();
    GLuint buf;
    glGenBuffers(1, &buf);
    bufferNames_[attribute] = buf;
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    
    //
    // GLProgram is specifying these locations but they are independently 
    // retrieved here. TODO: how should this be done?
    //
    GLint loc = glGetAttribLocation(programID_, attribute.c_str());
    if (loc < 0) 
    {
        throw std::runtime_error("couldn't find location for attribute: " + attribute);
    }
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer((GLuint)loc, 
                          (GLint)data.rows(), // dimension of points.
                          GL_FLOAT, 
                          GL_FALSE,           // normalise
                          0,                  // stride
                          0);                 // offset in currently bound GL_ARRAY_BUFFER
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);         // release binding
    glBindVertexArray(0);                     // release binding
}

inline
void GLVertexArrayObject::elementBuffer(std::vector<GLuint> const& indices)
{
    glBindVertexArray(vaoID_);
    
    numIndices = (GLsizei)indices.size();
    GLuint bufID;
    glGenBuffers(1, &bufID);
    bufferNames_["elementIndices"] = bufID;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,

                 sizeof(GLuint) * numIndices,

                 indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

//
// Texture buffer
//
template <typename T>
inline
void GLVertexArrayObject::texture1D(T const& image)
{    
    GLuint texID;
    glGenTextures(1, &texID);
    bufferNames_["textureBuffer"] = texID;
    glBindTexture(GL_TEXTURE_1D, texID);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R8, (GLsizei)image.size(),
                 0, GL_RED, GL_UNSIGNED_BYTE, image.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_1D, 0);
}

//
// Texture buffer
//
template <typename T>
inline
void GLVertexArrayObject::texture2D(T const& image)
{
    //
    // TEMPORARY: the texture buffer map is being abused.
    //
    
    GLuint texID;
    glGenTextures(1, &texID);

    bufferNames_["textureBuffer"] = texID;
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)image.cols(),
                 (GLsizei)image.rows(), 0,
                 GL_RED, GL_UNSIGNED_BYTE, image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

inline
GLuint GLVertexArrayObject::operator[](const std::string& attribute) const
{
    return bufferNames_.at(attribute);
}

inline
void GLVertexArrayObject::draw()
{
    glBindVertexArray(vaoID());
    
    // Binds any texture.
    //
    auto keyValPtr(bufferNames_.find("textureBuffer"));
    if (keyValPtr != bufferNames_.end())
    {
        glBindTexture(GL_TEXTURE_2D, keyValPtr->second);
    }
    
    // If we have an element buffer we drawElements.
    //
    if (numIndices > 0)
    {
        if (drawCallback)
        {
            drawCallback();
        }
        else
        {
            glDrawElements(primitiveType, numIndices, GL_UNSIGNED_INT, 0);
        }

    }

    // Otherwise, draw arrays.
    //
    else
    {	
        glDrawArrays(primitiveType, 0, numElements);
    }
}




















