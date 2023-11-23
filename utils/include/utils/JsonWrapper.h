//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include "BaseLog.h"
#include  <iostream>
using json = nlohmann::json;

namespace EA::EACC::Utils
{
	class  JsonWrapper 
	{
	private:
		json root;

	public:	
		JsonWrapper(){}
		~JsonWrapper() { }

		//Open a single json file and parse it
		void OpenFile(const char* path) { OpenFiles({ path }); }

		//Open multiple files and parse them, mergin them in the order of the vector
		void OpenFiles(const std::vector<std::string>& paths)
		{
			for (std::string path : paths) {
				std::ifstream file(path);
				if (file)
				{
					try
					{
						//Read contents of file in local parsed json
						json parsed = json::parse(file, nullptr, true, true); //ignores comments on json files
						
						if (root.empty())
						{
							root = parsed;
						}
						else
						{
							root.update(parsed);
						}
					}
					catch (json::exception& e)
					{
						LOG_CORE_ERROR(e.what());
					}
				}
				else
				{
					LOG_CORE_ERROR("{0} file could not be found", path);
				}
			}
		}

		void WriteFile(const char* path)
		{
			std::ofstream file(path);

			try
			{
				file << root;
				file.close();
			}
			catch(...)
			{
				LOG_CORE_ERROR("{0} file could not be written", path);
			}

		}

		template <class T>
		void SetVector(const char* section, const char* param, std::vector<T> value)
		{
			try
			{
				root[section][param] = value;
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not serialize {0} param from {1] section", param, section);
				throw;
			}
		}

		template <class T>
		void SetParam(const char* section, const char* param, T value)
		{
			try
			{
				root[section][param] = value;
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not serialize {0} param from {1] section", param, section);
				throw;
			}
		}

		template <class T>
		void SetParam(const char* param, T value)
		{
			try
			{
				root[param] = value;
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not serialize {0} param", param);
				throw;
			}
		}

		/// <summary>
		/// Obtains a specific section of the Json file
		/// </summary>
		/// <param name="section">Section to obtain from the Json file</param>
		/// <returns>Pointer to the desired section</returns>
		json* GetSection(const char* section) { return &(root)[section]; }

		/// <summary>
		/// Loads a vector from Json file
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="section">Section to read from</param>
		/// <param name="param">Parameter from section to load</param>
		/// <returns>Loaded vector</returns>
		template <class T>
		std::vector<T> GetVector(const char* section, const char* param)
		{
			try
			{
				return root[section][param].get<std::vector<T>>();
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not load {0} vector from {1] section", param, section);
				throw;
			}
		}

		/// <summary>
		/// Loads a parameter from Json file
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="section">Section to read from</param>
		/// <param name="param">Parameter from section to load</param>
		/// <returns>Loaded parameter</returns>
		template <class T>
		T GetParam(const char* section, const char* param)
		{
			try
			{
				return root[section][param];
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not load {0} param from {1] section", param, section);
				throw;
			}
		}

		/// <summary>
		/// Loads a parameter from Json file
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="param">Parameter from section to load</param>
		/// <returns>Loaded parameter</returns>
		template <class T>
		T GetParam(const char* param)
		{
			try
			{
				return root[param];
			}
			catch (...)
			{
				LOG_CORE_ERROR("Could not load {0} param", param);
				throw;
			}
		}

		/// <summary>
		/// Returns true if json entry is defined in json file
		/// </summary>
		/// <param name="param">json entry to check if it's contained</param>
		bool ContainsParam(const char* param)
		{
			return root.contains(param);
		}

		/// <summary>
		/// Returns true if json entry is defined in json file
		/// </summary>
		/// <param name="section">json section in which to check entry</param>
		/// <param name="param">json entry to check if it's contained</param>
		bool ContainsParam(const char* section, const char* param)
		{
			return root[section].contains(param);
		}
	};

}
