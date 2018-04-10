#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

#define CVLog(level) *OpenCVLogger::create(OpenCVLogger::LOG_##level, __FILE__, __LINE__)

/*
 * https://xkcd.com/color/rgb/
 */
const static cv::Scalar Color_BLACK = CV_RGB(0x00, 0x00, 0x00);
const static cv::Scalar Color_GRAY = CV_RGB(0x92, 0x95, 0x91);
const static cv::Scalar Color_WHITE = CV_RGB(0xFF, 0xFF, 0xFF);
const static cv::Scalar Color_RED = CV_RGB(0xE5, 0x00, 0x00);
const static cv::Scalar Color_RED_LIGHT = CV_RGB(0xFF, 0x47, 0x4C);
const static cv::Scalar Color_RED_FLAG = CV_RGB(0xAE, 0x1C, 0x28);
const static cv::Scalar Color_RED_DARK = CV_RGB(0x84, 0x00, 0x00);
const static cv::Scalar Color_GREEN = CV_RGB(0x15, 0xB0, 0x1A);
const static cv::Scalar Color_GREEN_BRIGHT = CV_RGB(0x01, 0xFF, 0x07);
const static cv::Scalar Color_GREEN_LIME = CV_RGB(0x89, 0xFE, 0x05);
const static cv::Scalar Color_GREEN_LIGHT = CV_RGB(0x96, 0xF9, 0x7B);
const static cv::Scalar Color_GREEN_FOREST = CV_RGB(0x06, 0x47, 0x0C);
const static cv::Scalar Color_GREEN_DARK = CV_RGB(0x03, 0x35, 0x00);
const static cv::Scalar Color_BLUE = CV_RGB(0x03, 0x43, 0xDF);
const static cv::Scalar Color_BLUE_LIGHT = CV_RGB(0x95, 0xD0, 0xFC);
const static cv::Scalar Color_BLUE_SKY = CV_RGB(0x75, 0xBB, 0xFD);
const static cv::Scalar Color_BLUE_FLAG = CV_RGB(0x21, 0x46, 0x8B);
const static cv::Scalar Color_BLUE_DARK = CV_RGB(0x00, 0x03, 0x5B);
const static cv::Scalar Color_PURPLE = CV_RGB(0x7E, 0x1E, 0x9C);
const static cv::Scalar Color_PURPLE_LIGHT = CV_RGB(0xBF, 0x77, 0xF6);
const static cv::Scalar Color_PURPLE_DARK = CV_RGB(0x35, 0x06, 0x3E);
const static cv::Scalar Color_PINK = CV_RGB(0xFF, 0x81, 0xC0);
const static cv::Scalar Color_YELLOW = CV_RGB(0xFF, 0xFF, 0x14);
const static cv::Scalar Color_ORANGE = CV_RGB(0xF9, 0x73, 0x06);
const static cv::Scalar Color_ORANGE_FLAG = CV_RGB(0xFF, 0x7F, 0x00);
const static cv::Scalar Color_BROWN = CV_RGB(0x65, 0x37, 0x00);
const static cv::Scalar Color_MAGENTA = CV_RGB(0xC2, 0x00, 0x78);
const static cv::Scalar Color_TEAL = CV_RGB(0x02, 0x93, 0x86);
const static cv::Scalar Color_TURQUOISE = CV_RGB(0x06, 0xC2, 0xAC);
const static cv::Scalar Color_VIOLET = CV_RGB(0x9A, 0x0E, 0xEA);
const static cv::Scalar Color_LAVENDER = CV_RGB(0xC7, 0x9F, 0xEF);
const static cv::Scalar Color_TAN = CV_RGB(0xD1, 0xB2, 0x6F);
const static cv::Scalar Color_CYAN = CV_RGB(0x00, 0xFF, 0xFF);
const static cv::Scalar Color_AQUA = CV_RGB(0x13, 0xEA, 0xC9);
const static cv::Scalar Color_MAUVE = CV_RGB(0xAE, 0x71, 0x81);
const static cv::Scalar Color_MAROON = CV_RGB(0x65, 0x00, 0x21);
const static cv::Scalar Color_OLIVE = CV_RGB(0x6E, 0x75, 0x0E);
const static cv::Scalar Color_COBALT = CV_RGB(0x1E, 0x48, 0x8F);
const static cv::Scalar Color_KHAKI = CV_RGB(0xAA, 0xA6, 0x62);

class OpenCVLogger
{
public:
	enum LogLevel
	{
		LOG_INFO, LOG_DEBUG, LOG_WARN, LOG_ERROR
	};
	enum LogFormat
	{
		FORMAT_DEFAULT, FORMAT_MATLAB, FORMAT_CSV, FORMAT_C, FORMAT_OPENCV
	};

private:
	struct Stream
	{
		Stream(LogLevel l) :
			c_log_level(l)
		{
		}

		const LogLevel c_log_level;
		std::ofstream ofs_file_buffer;
		std::stringstream ss_buffer;
	};

	std::shared_ptr<Stream> m_stream;

	static LogLevel Level;
	const size_t m_reference_width;
	bool m_singular;
	std::vector<size_t> m_channel_widths;
	int m_matrix_type;
	int m_dimension;
	int m_dimensions;
	size_t m_size;
	size_t m_precision;
	LogFormat m_output_format;

	typedef std::vector<std::pair<int, std::string>> LImageTSMap;
	typedef std::unique_ptr<LImageTSMap> LImageTSMapPtr;
	static const LImageTSMapPtr ImageTypeStringMapping;
	static const LImageTSMapPtr ImagePrimitiveStringMapping;
	static const LImageTSMap initTypeStringMapping();
	static const LImageTSMap initPrimitiveStringMapping();
	static const std::string &getMatDepthFromCode(int);
	static const std::string &getMatPrimitiveFromCode(int);

	std::string getDigitWidth(double mVal)
	{
		std::stringstream layout, ssize;
		layout << "%0." << (int)m_precision << "f";

		if (mVal != 0.f)
		{
			const int buffer_size = 256;
			char buffer[buffer_size];
			sprintf_s(buffer, buffer_size, layout.str().c_str(), mVal);

			ssize << buffer;
		}
		else
		{
			ssize << "0";
		}

		return ssize.str();
	}

	template<typename T, int c>
	std::vector<size_t> getValueWidth(const cv::Vec<T, c> &v, const cv::Mat &m)
	{
		std::vector<size_t> widths;

		for (int i = 0; i < m.channels(); ++i)
		{
			T value = 0;
			cv::Mat channel_vals;
			for (int r = 0; r < m.rows; ++r)
			{
				cv::Vec<T, c> v = m.at<cv::Vec<T, c> >(0, r);
				value = v[i];
				channel_vals.push_back(value);
			}

			std::vector<size_t> width = getValueWidth(value, channel_vals);

			widths.push_back(width.front());
		}

		return widths;
	}

	std::vector<size_t> getValueWidth(const double, const cv::Mat &m)
	{
		double minVal = DBL_MAX;
		double maxVal = -DBL_MAX;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::string ssize_min, ssize_max;
		ssize_min = getDigitWidth(minVal);
		ssize_max = getDigitWidth(maxVal);

		return std::vector<size_t>(1, std::max<size_t>(ssize_min.length(), ssize_max.length()));
	}

	std::vector<size_t> getValueWidth(const float, const cv::Mat &m)
	{
		double minVal = DBL_MAX;
		double maxVal = -DBL_MAX;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::string ssize_min, ssize_max;
		ssize_min = getDigitWidth(minVal);
		ssize_max = getDigitWidth(maxVal);

		return std::vector<size_t>(1, std::max<size_t>(ssize_min.length(), ssize_max.length()));
	}

	std::vector<size_t> getValueWidth(const uchar, const cv::Mat &m)
	{
		double minVal = DBL_MAX;
		double maxVal = -DBL_MAX;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::stringstream ssize_min, ssize_max;
		ssize_min << (int)minVal;
		ssize_max << (int)maxVal;

		return std::vector<size_t>(1, std::max<size_t>(ssize_min.str().length(), ssize_max.str().length()));
	}

	std::vector<size_t> getValueWidth(const schar, const cv::Mat &m)
	{
		double minVal = DBL_MAX;
		double maxVal = -DBL_MAX;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::stringstream ssize_min, ssize_max;
		ssize_min << (int)minVal;
		ssize_max << (int)maxVal;

		return std::vector<size_t>(1, std::max<size_t>(ssize_min.str().length(), ssize_max.str().length()));
	}

	template<typename T>
	std::vector<size_t> getValueWidth(const T &, const cv::Mat &m)
	{
		double minVal = DBL_MAX;
		double maxVal = -DBL_MAX;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::stringstream ssize_min, ssize_max;
		ssize_min << (T)minVal;
		ssize_max << (T)maxVal;

		return std::vector<size_t>(1, std::max<size_t>(ssize_min.str().length(), ssize_max.str().length()));
	}

	template<typename T>
	size_t getValueWidth(const T &value)
	{
		std::stringstream ssize;
		ssize << (T)value;

		return ssize.str().length();
	}

	template<typename T>
	inline void doIntegerInputMarkup(const T &input, const std::string &type)
	{
		if (!m_singular)
		{
			const int buffer_size = 64;
			char buffer[buffer_size];
			sprintf_s(buffer, buffer_size, type.c_str(), input);

			std::string space;
			std::string sbuffer = std::string(buffer);
			if (m_size > 0) space = std::string(std::max<int>(0, (int)m_size - (int)sbuffer.length()), ' ');

			*this << space << buffer;
		}
		else
		{
			const int buffer_size = 64;
			char buffer[buffer_size];
			sprintf_s(buffer, buffer_size, type.c_str(), input);

			*this << buffer;
		}
	}

	template<typename T>
	inline void doRealInputMarkup(const T &input, const std::string &type)
	{
		if (!m_singular)
		{
			if (input != 0.f)
			{
				*this << getRealInputMarkUp(input, "f");
			}
			else
			{
				std::string space(m_size - 1, ' ');
				*this << space << "0";
			}
		}
		else
		{
			size_t size = m_size;
			m_size = 0;
			*this << getRealInputMarkUp(input, "f");
			m_size = size;
		}
	}

	template<typename T>
	inline std::string getRealInputMarkUp(const T &input, const std::string &type)
	{
		std::stringstream layout;
		layout << "%" << (int)m_size << "." << (int)m_precision << type;

		const int buffer_size = 256;
		char buffer[buffer_size];
		sprintf_s(buffer, buffer_size, layout.str().c_str(), input);

		return buffer;
	}

	template<typename T>
	OpenCVLogger& operator<<(cv::Mat_<T>& matrix)
	{
		cv::Mat t_m = matrix;

		if (t_m.dims < 3)
		{
			// display 2D matrix

			size_t s = m_size;
			m_size = 0;

			switch (m_output_format)
			{
			case FORMAT_OPENCV:
				*this << "cv::Mat var = (cv::Mat_<" << getMatPrimitiveFromCode(m_matrix_type) << " >";
				*this << "(" << t_m.rows << ", " << t_m.cols << ") << \\\n";
				break;
			case FORMAT_MATLAB:
			case FORMAT_CSV:
				break;
			case FORMAT_DEFAULT:
			default:
				*this << getMatDepthFromCode(matrix.type()) << "(" << t_m.rows << "x" << t_m.cols << ")\n";
				break;
			}

			m_size = s;
			std::vector<std::vector<size_t>> size;

			for (int y = 0; y < t_m.rows; y++)
			{
				switch (m_output_format)
				{
				case FORMAT_MATLAB:
				{
					if (y > 0)
						*this << "  ";
					else
						*this << "  ";
					break;
				}
				case FORMAT_C:
				{
					if (y > 0)
						*this << "  ";
					else
						*this << "{ ";
					break;
				}
				case FORMAT_CSV:
					break;
				case FORMAT_DEFAULT:
					*this << "\t";
					break;
				default:
					break;
				}

				for (int x = 0; x < t_m.cols; x++)
				{
					if (y == 0)
					{
						cv::Mat row = t_m.col(x);
						T& wp = matrix(cv::Point(0, 0));
						size.push_back(getValueWidth(wp, row));
					}

					m_channel_widths = size.at(x);
					m_size = m_channel_widths.front();
					T& point = matrix(cv::Point(x, y));

					switch (m_output_format)
					{
					case FORMAT_OPENCV:
					{
						if (y == t_m.rows - 1 && x == t_m.cols - 1)
							*this << point;
						else if (x == 0)
							*this << "               " << point << ", ";
						else if (x == t_m.cols - 1)
							*this << point << ", \\";
						else
							*this << point << ", ";
						break;
					}
					case FORMAT_MATLAB:
					{
						if (x == t_m.cols - 1)
							*this << point;
						else
							*this << point << " ";
						break;
					}
					case FORMAT_C:
					{
						if (y == t_m.rows - 1 && x == t_m.cols - 1)
							*this << point << " ";
						else if (x == t_m.cols - 1)
							*this << point << ", \\";
						else
							*this << point << ", ";
						break;
					}
					case FORMAT_CSV:
					{
						if (x == t_m.cols - 1)
							*this << point;
						else
							*this << point << "; ";
						break;
					}
					case FORMAT_DEFAULT:
					default:
						*this << point << " ";
						break;
					}
				}

				switch (m_output_format)
				{
				case FORMAT_OPENCV:
				{
					if (y != t_m.rows - 1) *this << "\n";
					break;
				}
				case FORMAT_MATLAB:
				{
					if (y != t_m.rows - 1) *this << ";\n";
					break;
				}
				case FORMAT_C:
				{
					if (y != t_m.rows - 1) *this << "\n";
					break;
				}
				case FORMAT_DEFAULT:
				default:
					*this << "\n";
					break;
				}
			}

			switch (m_output_format)
			{
			case FORMAT_OPENCV:
				*this << ");\n";
				break;
			case FORMAT_MATLAB:
			{
				if (m_dimension != m_dimensions - 1)
					*this << ",\n";
				else
					*this << "]";
				break;
			}
			case FORMAT_C:
				*this << "}\n";
				break;
			case FORMAT_DEFAULT:
			default:
				break;
			}

			m_size = s;
		}
		else
		{
			// handle high dimensional matrices
			CV_Assert(m_size > 0);

			size_t s = m_size;
			m_size = 0;
			for (int d = 0; d < t_m.dims; ++d)
			{
				*this << t_m.size[d];
				if (d + 1 == t_m.dims)
					*this << "\n";
				else
					*this << "x";
			}
			switch (m_output_format)
			{
			case FORMAT_MATLAB:
				*this << "reshape([ ";
				break;
			default:
				break;
			}
			m_size = s;

			int d = 0;
			m_dimensions = t_m.size[d];
			for (int c = 0; c < t_m.size[d]; ++c)
			{
				m_dimension = c;
				size_t s = m_size;
				m_size = 0;
				switch (m_output_format)
				{
				case FORMAT_OPENCV:
					break;
				case FORMAT_MATLAB:
					break;
				case FORMAT_CSV:
					break;
				case FORMAT_DEFAULT:
				default:
					*this << c << "; ";
					break;
				}

				m_size = s;

				std::vector<cv::Range> range;
				for (int rd = 0; rd < t_m.dims; ++rd)
				{
					if (rd == d)
						range.push_back(cv::Range(c, c + 1));
					else
						range.push_back(cv::Range::all());
				}

				cv::Mat p = t_m(&range[0]).clone();
				std::vector<int> sizedmin;
				for (int dmin = d + 1; dmin < t_m.dims; ++dmin)
					sizedmin.push_back(t_m.size[dmin]);

				cv::Mat pdmin;
				pdmin.create(t_m.dims - 1, &sizedmin[0], p.type());
				pdmin.data = p.data;

				*this << pdmin;

				if (c != t_m.size[d] - 1 && d != t_m.dims - 1) *this << "\n";
				/*
				switch (_output_format)
				{
				default:

				break;
				}
				*/
			}

			m_size = 0;
			switch (m_output_format)
			{
			case FORMAT_MATLAB:
				*this << ",[";
				for (int d = 0; d < t_m.dims; ++d)
					*this << t_m.size[d] << " ";
				*this << "])";
				break;
			default:
				break;
			}
			m_size = s;
		}

		return *this;
	}

public:
	OpenCVLogger(LogLevel);
	virtual ~OpenCVLogger();

	static std::shared_ptr<OpenCVLogger> create(
		const OpenCVLogger::LogLevel, const std::string, const int = 0);
	static std::string getLevelDescr(
		LogLevel);

	OpenCVLogger& operator<<(const char*);
	OpenCVLogger& operator<<(uchar);
	OpenCVLogger& operator<<(schar);
	OpenCVLogger& operator<<(double);
	OpenCVLogger& operator<<(float);
	OpenCVLogger& operator<<(long);
	OpenCVLogger& operator<<(int);
	OpenCVLogger& operator<<(short);
	OpenCVLogger& operator<<(ushort);
	OpenCVLogger& operator<<(bool);
	OpenCVLogger& operator<<(long unsigned int);
	OpenCVLogger& operator<<(const std::stringstream&);
	OpenCVLogger& operator<<(const cv::Mat&);
	OpenCVLogger& operator<<(const cv::MatExpr&);
	OpenCVLogger& operator<<(const cv::SparseMat&);
	OpenCVLogger& operator<<(const cv::Size&);
	OpenCVLogger& operator<<(const cv::Scalar&);
	OpenCVLogger& operator<<(const cv::Point&);
	OpenCVLogger& operator<<(const cv::Rect&);
	OpenCVLogger& operator<<(const cv::Range&);

	template<typename T>
	OpenCVLogger& operator<<(const T& input)
	{
		std::stringstream buffer;
		buffer << input;
		*this << buffer.str().c_str();
		return *this;
	}

	template<typename T>
	OpenCVLogger& operator<<(const std::vector<T>& input)
	{
		m_singular = true;
		typename std::vector<T>::const_iterator iter = input.begin();
		*this << "(" << input.size() << "):";

		for (size_t i = 0; i < input.size(); i++, ++iter)
			*this << *iter << ", ";

		return *this;
	}

	size_t getReferenceWidth()
	{
		return m_reference_width;
	}
};

class Utilities
{
public:
	Utilities();
	virtual ~Utilities();

	static void getDirectory(
		const std::string &, const std::string &, std::vector<std::string> &);

	static std::string getBasename(
		const std::string &);
	static std::string getExtension(
		const std::string &);
};

