#ifndef LIBCOMBINE_KNOWNFORMATS_H
#define LIBCOMBINE_KNOWNFORMATS_H

#include <omex/common/extern.h>

#ifdef __cplusplus

#include <map>
#include <vector>
#include <string>

LIBCOMBINE_CPP_NAMESPACE_BEGIN

class LIBCOMBINE_EXTERN KnownFormats
{
public:
  /**
   * this function tests whether the provided format matches
   * the given formatKey
   *
   * @param formatKey the key
   * @param format the full format string
   *
   * @return boolean indicating whether the format matches the
   *         key or not.
   */
  static bool isFormat(const std::string& formatKey,
                       const std::string& format);


  /**
   * returns the first format string for the given key
   *
   * @param formatKey the key
   *
   * @return the found format string or empty string if not found.
   */
  static std::string lookupFormat(const std::string &formatKey);

  /**
   * Guesses the format from the filename
   *
   * @param fileName the filename, if it is an xml file, the first 128
   *        characters will be looked through to find some of the known
   *        xml formats.
   *
   * @return the format if found, empty string otherwise
   */
  static std::string guessFormat(const std::string& fileName);


  /**
   * adds a new entry to the list of known formats
   *
   * @param formatKey the key for the new known format
   * @param format the format string (mime type or identifiers.org reference)
   *
   */
  static void addKnownFormat(const std::string& formatKey, const std::string& format);
  
  /**
   * @return a vector of all known format keys
   */
  static std::vector< std::string > getFormatKeys();

  /**
   * @return the map of known format 
   */
  static std::map<std::string, std::vector<std::string> >& getKnownFormats();


protected:
  static std::map<std::string, std::vector<std::string> > initializeMap();
  static std::map<std::string, std::vector<std::string> > mKnownFormats;
};

LIBCOMBINE_CPP_NAMESPACE_END

#endif  /* __cplusplus */

#endif  /* LIBCOMBINE_KNOWNFORMATS_H */
