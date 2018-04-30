
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{
	class Exception : public std::exception
    {
    public:
		Exception(const char* type, const std::string& description, const std::string& source)
			:mTypeName(type), mDescription(description), mSource(source)
		{ }

		Exception(const char* type, const std::string& description, const std::string& source, const char* file, long line)
			: mLine(line), mTypeName(type), mDescription(description), mSource(source), mFile(file)
		{ }

        Exception(const Exception& rhs)
			: mLine(rhs.mLine), mTypeName(rhs.mTypeName), mDescription(rhs.mDescription),
			mSource(rhs.mSource), mFile(rhs.mFile)
		{ }

		~Exception() noexcept = default;

        void operator = (const Exception& rhs)
		{
			mDescription = rhs.mDescription;
			mSource = rhs.mSource;
			mFile = rhs.mFile;
			mLine = rhs.mLine;
			mTypeName = rhs.mTypeName;
		}
		
		virtual const std::string& getFullDescription() const
		{
			if (mFullDesc.empty())
			{
				std::stringstream desc;
				

				desc << "BANSHEE EXCEPTION(" << mTypeName << "): "
					<< mDescription
					<< " in " << mSource;

				if (mLine > 0)
				{
					desc << " at " << mFile << " (line " << mLine << ")";
				}

				mFullDesc = desc.str();
			}

			return mFullDesc;
		}
		
		virtual const std::string& getSource() const { return mSource; }
		
		virtual const std::string& getFile() const { return mFile; }
        
        virtual long getLine() const { return mLine; }
		
		virtual const std::string& getDescription(void) const { return mDescription; }
		
		const char* what() const noexcept override { return getFullDescription().c_str(); }

	protected:
		long mLine = 0;
		std::string mTypeName;
		std::string mDescription;
		std::string mSource;
		std::string mFile;
		mutable std::string mFullDesc;
    };
	
	class NotImplementedException : public Exception 
	{
	public:
		NotImplementedException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("NotImplementedException", inDescription, inSource, inFile, inLine) {}
	};
	
	class FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("FileNotFoundException", inDescription, inSource, inFile, inLine) {}
	};

	class IOException : public Exception
	{
	public:
		IOException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("IOException", inDescription, inSource, inFile, inLine) {}
	};
	
	class InvalidStateException : public Exception
	{
	public:
		InvalidStateException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InvalidStateException", inDescription, inSource, inFile, inLine) {}
	};	

	class InvalidParametersException : public Exception
	{
	public:
		InvalidParametersException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InvalidParametersException", inDescription, inSource, inFile, inLine) {}
	};

	class InternalErrorException : public Exception
	{
	public:
		InternalErrorException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InternalErrorException", inDescription, inSource, inFile, inLine) {}
	};
	
	class RenderingAPIException : public Exception
	{
	public:
		RenderingAPIException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("RenderingAPIException", inDescription, inSource, inFile, inLine) {}
	};

	class UnitTestException : public Exception
	{
	public:
		UnitTestException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("UnitTestException", inDescription, inSource, inFile, inLine) {}
	};

#ifndef NCT_EXCEPT
#define NCT_EXCEPT(type, desc)	\
		{                       \
	      static_assert((std::is_base_of<Nct::Exception, type>::value), "Invalid exception type (" #type ") for NCT_EXCEPT macro. It needs to derive from NCT::Exception."); \
          throw type(desc,__FUNCTION__, __FILE__, __LINE__); \
	    }
#endif
	
}

