#include <iostream>
#include "btree.h"
#include <chrono>
#include <fstream>
#include "args.hxx"
#include <vector>
#include "util.h"

using namespace std;

int main(int argc, char *argv[])
{
    args::ArgumentParser parser("Sample STX Btree Test Parser.", "");

    args::Group group1(parser, "These arguments are REQUIRED",
                       args::Group::Validators::DontCare);

    args::ValueFlag<std::string> input_file_cmd(group1, "input_file_path", "Path to input file", {'f', "input_file_path"});
    args::ValueFlag<std::string> output_file_cmd(group1, "output_file_path", "Path to output file", {'f', "output_file_path"});

    if (argc == 1)
    {
        std::cout << parser;
        exit(0);
    }
    try
    {
        parser.ParseCLI(argc, argv);

        string input_file = args::get(input_file_cmd);

        std::vector<pair<unsigned long, string>> data;

        string line, word, temp;
        vector<string> row;
        cout << "now starting to read" << endl;

        string file_contents = readFileIntoString(input_file);
        istringstream sstream(file_contents);
        string record;
        int i = 0;
        while (std::getline(sstream, record))
        {
            row.clear();
            istringstream line(record);
            while (std::getline(line, record, ','))
            {
                row.push_back(record);
            }

            // first word of every row is an unsigned long
            unsigned long key = stoul(row[0]);

            // add to our data vector
            data.push_back(make_pair(key, row[1]));
        }
        cout << "read " << data.size() << " rows from file" << endl;
        stx::btree<unsigned long, string> tree;
        cout << "Now inserting data" << endl;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < data.size(); i++)
        {
            tree.insert(data[i].first, data[i].second);
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_sort = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        cout << "Time taken for inserts = " << duration_sort << " nanoseconds" << endl;
        cout << "Inserted " << data.size() << " rows." << endl;
        // for (int i = 0; i < data.size(); i++)
        // {
        //     cout << "Key = " << data[i].first << " : " << tree.exists(data[i].first)<<endl;
        // }
    }
    catch (args::Help &)
    {
        std::cout << parser;
        exit(0);
        // return 0;
    }
    catch (args::ParseError &e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError &e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    return 0;
}