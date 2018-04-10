// Utilities.cpp : Defines the entry point for the console application.
// Coert van Gemeren (2017) 
// Use this code under the following terms:
// Attribution: appropriate credit + indicate if changes were made
#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <string>
#include <iostream>
#pragma comment(lib, "User32.lib")

Utilities::Utilities()
{
}

Utilities::~Utilities()
{
}

void Utilities::getDirectory(
	const std::string &path, const std::string &mask, std::vector<std::string> &files)
{
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::wstring stemp = std::wstring(path.begin(), path.end());

	StringCchCopy(szDir, MAX_PATH, stemp.c_str());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	StringCchLength(stemp.c_str(), MAX_PATH, &length_of_arg);
	assert(length_of_arg < MAX_PATH);

	hFind = FindFirstFile(szDir, &ffd);

	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::wstring tmp(ffd.cFileName);
			std::string filename(tmp.begin(), tmp.end());

			if (mask.empty() || getExtension(filename) == mask)
			{
				files.emplace_back(path + "/" + filename);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);
}

std::string Utilities::getBasename(
	const std::string &filename)
{
	std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	return base_filename.substr(0, p);
}

std::string Utilities::getExtension(
	const std::string &filename)
{
	return filename.substr(filename.find_last_of(".") + 1);
}

const OpenCVLogger::LImageTSMapPtr OpenCVLogger::ImageTypeStringMapping =
	LImageTSMapPtr(new OpenCVLogger::LImageTSMap(OpenCVLogger::initTypeStringMapping()));
const OpenCVLogger::LImageTSMapPtr OpenCVLogger::ImagePrimitiveStringMapping =
	LImageTSMapPtr(new OpenCVLogger::LImageTSMap(OpenCVLogger::initPrimitiveStringMapping()));

OpenCVLogger::OpenCVLogger(LogLevel level) :
	m_stream(std::make_shared<Stream>(level)),
	m_reference_width(22),
	m_singular(true),
	m_size(8),
	m_precision(5),
	m_matrix_type(CV_8U),
	m_output_format(FORMAT_DEFAULT)
{
}

OpenCVLogger::~OpenCVLogger()
{
	const std::string input = m_stream->ss_buffer.str();

	if (m_stream->c_log_level > LOG_WARN)
	{
		std::cerr << input << std::endl;
	}
	else if (m_stream->c_log_level == LOG_ERROR)
	{
		std::cerr << input << std::endl;
	}
	else if (m_stream->c_log_level == LOG_DEBUG)
	{
		std::cout << input << std::endl;
	}
	else if (m_stream->c_log_level == LOG_INFO)
	{
		std::cout << input << std::endl;
	}
}

std::shared_ptr<OpenCVLogger> OpenCVLogger::create(const OpenCVLogger::LogLevel level, const std::string file, const int line)
{
	std::shared_ptr<OpenCVLogger> logger = std::make_shared<OpenCVLogger>(level);

	std::stringstream __log_meta;

	if (line > 0)
	{
		int __log_maxwidth = (int)logger->getReferenceWidth();
		std::stringstream __log_msg_line;
		__log_msg_line << Utilities::getBasename(file) + "." + Utilities::getExtension(file) << ":" << line;
		std::string __log_msg = __log_msg_line.str();
		int __log_length = MIN(__log_maxwidth, (int)__log_msg.length());
		std::string __trimmed = __log_msg.substr(__log_msg.length() - __log_length, __log_length);
		std::string __log_pad;
		int __log_pad_length = MAX(__log_maxwidth - (int)__trimmed.length(), 0);
		if (__log_pad_length > 0) __log_pad.insert(0, __log_pad_length, ' ');
		__log_meta << __log_pad << __trimmed;
	}

	*logger << "[" << __log_meta.str() << "]\t";
	return logger;
}

std::string OpenCVLogger::getLevelDescr(LogLevel level)
{
	switch (level)
	{
	case LOG_INFO:
		return "INFO ";
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_WARN:
		return "WARN ";
	case LOG_ERROR:
		return "ERROR";
	default:
		return "INFO ";
	}
}

const OpenCVLogger::LImageTSMap OpenCVLogger::initTypeStringMapping()
{
	LImageTSMap imageTypeStringMapping;

	imageTypeStringMapping.reserve(19);
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(-1, (std::string) "CV_???"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8U, (std::string) "CV_8U"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC2, (std::string) "CV_8UC2"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC3, (std::string) "CV_8UC3"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC4, (std::string) "CV_8UC4"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8S, (std::string) "CV_8S"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_16U, (std::string) "CV_16U"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_16S, (std::string) "CV_16S"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32S, (std::string) "CV_32S"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32F, (std::string) "CV_32F"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC2, (std::string) "CV_32FC2"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC3, (std::string) "CV_32FC3"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC4, (std::string) "CV_32FC4"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC(5), (std::string) "CV_32FC5"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64F, (std::string) "CV_64F"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC2, (std::string) "CV_64FC2"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC3, (std::string) "CV_64FC3"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC4, (std::string) "CV_64FC4"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC(5), (std::string) "CV_64FC5"));

	return imageTypeStringMapping;
}

const OpenCVLogger::LImageTSMap OpenCVLogger::initPrimitiveStringMapping()
{
	LImageTSMap imageTypeStringMapping;

	imageTypeStringMapping.reserve(19);
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(-1, (std::string) "CV_???"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8U, (std::string) "uchar"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC2, (std::string) "cv::Vec2b"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC3, (std::string) "cv::Vec3b"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8UC4, (std::string) "cv::Vec4b"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_8S, (std::string) "schar"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_16U, (std::string) "ushort"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_16S, (std::string) "short"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32S, (std::string) "int"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32F, (std::string) "float"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC2, (std::string) "cv::Vec2f"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC3, (std::string) "cv::Vec3f"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC4, (std::string) "cv::Vec4f"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_32FC(5), (std::string) "cv::Vec<float, 5>"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64F, (std::string) "double"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC2, (std::string) "cv::Vec2d"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC3, (std::string) "cv::Vec3d"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC4, (std::string) "cv::Vec4d"));
	imageTypeStringMapping.emplace_back(std::pair<int, std::string>(CV_64FC(5), (std::string) "cv::Vec<double, 5>"));

	return imageTypeStringMapping;
}

const std::string &OpenCVLogger::getMatDepthFromCode(int depth)
{
	for (size_t i = 1; i < ImageTypeStringMapping->size(); ++i)
	{
		if (ImageTypeStringMapping->at(i).first == depth)
		{
			return ImageTypeStringMapping->at(i).second;
		}
	}
	return ImageTypeStringMapping->front().second;
}

const std::string &OpenCVLogger::getMatPrimitiveFromCode(int depth)
{
	for (size_t i = 1; i < ImagePrimitiveStringMapping->size(); ++i)
	{
		if (ImagePrimitiveStringMapping->at(i).first == depth)
		{
			return ImagePrimitiveStringMapping->at(i).second;
		}
	}
	return ImageTypeStringMapping->front().second;
}

OpenCVLogger& OpenCVLogger::operator<<(const char* input)
{
	assert(m_stream != nullptr);
	m_stream->ss_buffer << input;
	return *this;
}

/*
* Cast unsigned char to int to show it as a number
*/
OpenCVLogger& OpenCVLogger::operator<<(uchar input)
{
	*this << (int)input;
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(schar input)
{
	doIntegerInputMarkup((int)input, "%d");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(short input)
{
	doIntegerInputMarkup(input, "%d");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(ushort input)
{
	doIntegerInputMarkup(input, "%u");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(int input)
{
	doIntegerInputMarkup(input, "%d");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(long unsigned int input)
{
	doIntegerInputMarkup(input, "%lu");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(long input)
{
	doIntegerInputMarkup(input, "%ld");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(float input)
{
	doRealInputMarkup(input, "f");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(double input)
{
	doRealInputMarkup(input, "f");
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(bool input)
{
	if (input)
		*this << "true";
	else
		*this << "false";
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const std::stringstream& input)
{
	*this << input.str();
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Point& input)
{
	*this << "(" << input.x << ";" << input.y << ")";
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Size& input)
{
	*this << "w:" << input.width << " x h:" << input.height;
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Scalar& input)
{
	for (int i = 0; i < input.channels; i++)
		*this << input[i] << ",";
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Rect& input)
{
	*this << input.x << "," << input.y << ":" << input.width << "x" << input.height;
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Range& range)
{
	bool s = m_singular;
	m_singular = true;
	*this << range.start << "<->" << range.end << " (" << range.size() << ")";
	m_singular = s;
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::MatExpr &mat)
{
	bool s = m_singular;
	cv::Mat casted(mat);
	*this << casted;
	m_singular = s;
	return *this;
}

OpenCVLogger& OpenCVLogger::operator<<(const cv::Mat &mat)
{
	bool s = m_singular;
	m_singular = false;
	m_matrix_type = mat.type();

	switch (m_matrix_type)
	{
	case CV_8U:
	{
		cv::Mat_<uchar> type = mat;
		*this << type;
		break;
	}
	case CV_8UC2:
	{
		cv::Mat_<cv::Vec2b> type = mat;
		*this << type;
		break;
	}
	case CV_8UC3:
	{
		cv::Mat_<cv::Vec3b> type = mat;
		*this << type;
		break;
	}
	case CV_8UC4:
	{
		cv::Mat_<cv::Vec4b> type = mat;
		*this << type;
		break;
	}
	case CV_8S:
	{
		cv::Mat_<schar> type = mat;
		*this << type;
		break;
	}
	case CV_8SC2:
	{
		cv::Mat_<cv::Vec2s> type = mat;
		*this << type;
		break;
	}
	case CV_8SC3:
	{
		cv::Mat_<cv::Vec3s> type = mat;
		*this << type;
		break;
	}
	case CV_8SC4:
	{
		cv::Mat_<cv::Vec4s> type = mat;
		*this << type;
		break;
	}
	case CV_16U:
	{
		cv::Mat_<ushort> type = mat;
		*this << type;
		break;
	}
	case CV_16S:
	{
		cv::Mat_<short> type = mat;
		*this << type;
		break;
	}
	case CV_32S:
	{
		cv::Mat_<int> type = mat;
		*this << type;
		break;
	}
	case CV_32SC2:
	{
		cv::Mat_<cv::Vec2i> type = mat;
		*this << type;
		break;
	}
	case CV_32SC3:
	{
		cv::Mat_<cv::Vec3i> type = mat;
		*this << type;
		break;
	}
	case CV_32SC4:
	{
		cv::Mat_<cv::Vec4i> type = mat;
		*this << type;
		break;
	}
	case CV_32F:
	{
		cv::Mat_<float> type = mat;
		*this << type;
		break;
	}
	case CV_32FC2:
	{
		cv::Mat_<cv::Vec2f> type = mat;
		*this << type;
		break;
	}
	case CV_32FC3:
	{
		cv::Mat_<cv::Vec3f> type = mat;
		*this << type;
		break;
	}
	case CV_32FC4:
	{
		cv::Mat_<cv::Vec4f> type = mat;
		*this << type;
		break;
	}
	case CV_64F:
	{
		cv::Mat_<double> type = mat;
		*this << type;
		break;
	}
	case CV_64FC2:
	{
		cv::Mat_<cv::Vec2d> type = mat;
		*this << type;
		break;
	}
	case CV_64FC3:
	{
		cv::Mat_<cv::Vec3d> type = mat;
		*this << type;
		break;
	}
	case CV_64FC4:
	{
		cv::Mat_<cv::Vec4d> type = mat;
		*this << type;
		break;
	}
	}

	m_singular = s;
	return *this;
}
