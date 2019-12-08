#include <using_convenience.hpp>

void print_content(ostream& out, path input_file)
{
	ifstream in(input_file, std::ios::binary);

	if (!in)
		throw runtime_error(str("Failed to open %1% for reading"_f %
			input_file));

	out << "\tstd::make_tuple(std::string(\"" << input_file.generic_string() <<
		"\"), std::array<char, " << file_size(input_file) << ">{";

	for (char c : istreambuf_range(in))
		out << (int) c << ",";

	out << "})";
}

template<class Range>
void directory_to_header(Range const& files, optional<string> output_file,
	optional<string> variable_name, optional<string> namespace_name)
{
	if (!variable_name)
		variable_name = "files";

	if (!namespace_name)
		namespace_name = "embedded_file";

	ofstream out(*output_file);

	if (!out)
		throw runtime_error(str("Failed to open %1% for writing"_f %
			*output_file));

	out << "#pragma once\n\n";
	out << "#include <array>\n";
	out << "#include <tuple>\n";
	out << "#include <string>\n\n";
	out << "namespace " << *namespace_name << " {\n";
	out << "auto " << *variable_name << " = std::make_tuple(\n";

	for (path file : files)
	{
		print_content(out, file);
		out << ",\n";
	}
	
	out << ");\n";
	out << "}\n";
}

void convert_to_header(path input_file, optional<string> output_file,
	optional<string> variable_name, optional<string> namespace_name)
{
	if (is_directory(input_file))
	{
		if (!exists(input_file))
			throw runtime_error(str("%1% does not exist.\n"_f %	input_file));

		if (!output_file)
			output_file = input_file.remove_trailing_separator().filename()
				.generic_string() + ".hpp";

		directory_to_header(recursive_path_iterator(input_file) |
				filtered(is_regular_file(_1)), output_file, variable_name,
			namespace_name);
	}
	else
	{
		vector<path> files;
		files += input_file;
		directory_to_header(files, output_file, variable_name, namespace_name);
	}
}

auto parse_commandline(int argc, char** argv)
{
	optional<string> input_file, output_file, variable_name, namespace_name;
	options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("input-file", po::value(&input_file),
			"The file to embed")
		("output-file", po::value(&output_file),
			"The header to embed in")
		("variable-name", po::value(&variable_name),
			"The name of the variable which holds the data")
		("namespace-name,n", po::value(&namespace_name),
			"The name of the namespace to define the data in");

	positional_options_description p;
	p.add("input-file", 1);
	p.add("output-file", 1);

	variables_map vm;

	try
	{
		po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
		po::notify(vm);
	}
	catch (exception const& e)
	{
		throw runtime_error(str("%1%\n%2%"_f % e.what() % desc));
	}

	if (!input_file)
		throw runtime_error(str("No input file specified.\n%1%"_f % desc));

	return make_tuple(*input_file, output_file, variable_name, namespace_name);
}

int main(int argc, char** argv)
{
	try
	{
		unpack(convert_to_header)(parse_commandline(argc, argv));
	}
	catch (exception const& e)
	{
		cerr << e.what();
		return -1;
	}

	return 0;
}
