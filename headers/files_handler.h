
void update_entries_file(const char *src_file, const char *symbol, int mem_address);
void update_externals_file(const char *src_file, const char *symbol, int ref_address);
void create_ob_file(const char *filename, CodeNode code_list, int IC, int DC);
int binary_to_octal(CodeNode code_list);
