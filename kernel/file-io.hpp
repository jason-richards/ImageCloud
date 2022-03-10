#include <fstream>
#include <memory>


class InputFile {
  std::ifstream m_File;

public:

  using InputFilePtr = std::shared_ptr<InputFile>;

  InputFile (const std::string& path)               { m_File = std::ifstream(path, std::ifstream::binary);}
 ~InputFile ()                                      { m_File.close();}

  static InputFilePtr Create(const std::string& p)  { return std::make_shared<InputFile>(p); }
  std::istream& Get()                               { return m_File; }
  std::istream& read(char * __s, size_t __n)        { return m_File.read(__s, __n); }
  size_t gcount() const                             { return m_File.gcount(); }
  int eof() const                                   { return m_File.eof(); }
};


class OutputFile {
  std::ofstream m_File;

public:

  using OutputFilePtr = std::shared_ptr<OutputFile>;

  OutputFile (const std::string& path)              { m_File = std::ofstream(path, std::ofstream::binary); }
 ~OutputFile ()                                     { m_File.close(); }

  static OutputFilePtr Create(const std::string& p) { return std::make_shared<OutputFile>(p); }
  std::ostream& Get()                               { return m_File; }
  std::ostream& write(const char * __s, size_t __n) { return m_File.write(__s, __n); }
};


