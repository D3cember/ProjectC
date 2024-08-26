int binary_to_octal(const char *binary_code);
void update_externals_file(const char *src_file, const char *symbol, int ref_address);
void create_entry_file(const char *filename, Symbol *symbol_table);
void create_ext_file(const char *base_filename, Symbol *symbol_table);
void create_ob_file(char *filename, CodeNode *code_list, int IC, int DC);
