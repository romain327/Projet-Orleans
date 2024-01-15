with Text_IO;
with Direct_IO;

package Principal is
	type uint8_t is mod 256;
	for uint8_t'Size use 8;
	
	type Ptr is access uint8_t;

	type table_t is
		record
			Name : String (1..12);
			Size : Integer;
			Reserved : Integer;
			Datetime_str : String(1..28);
		end record;
		
	procedure Print_table_t(structure : table_t);
	function get_nb_struct(F : Direct_IO.File_Type; File_Name : String) return Integer;
	function main() return Integer;
end Principal;

package body Principal is
	procedure Print_table_t(structure : table_t) is
	begin
		Put_Line("Nom : " & table_t.Name);
		Put_Line("Taille : " & Integer'Image(table_t.Size)); 
		Put_Line("Reserved : " & Integer'Image(table_t.Reserved));
		Put_Line("Datetime : ") & table_t.Datetime_str);
	end Print_table_t;
	
	function get_nb_struct(F : Direct_IO.File_Type, File_Name : String) is
	    Struct_nb : Integer := 0;
	    File_In : Direct_IO.File_Mode := Direct_IO.In_File;
	    C : uint8_t := 16#0#;
	    Ptr_c : Ptr := C.all'address;
    begin
		Direct_IO.Open(F, File_In, File_Name);
        Direct_IO.Read(F, Ptr_c, 1);
        while C = 16#FF# loop
            Direct_IO.Read(F, Ptr_c, 1);
            Struct_nb := Struct_nb + 1;
            Direct_IO.Set_Index(F, Direct_IO.Index(F) + table_t'Size);
        end loop;
        Put_Line("Nombre de structures : " & Integer'Image(Struct_nb));

        return Struct_nb;

	exception
		when Name_Error =>
			Put_Line("File does not exists.");
		when others =>
			Put_Line("Error when processing" & File_Name);
    end get_nb_struct;

    function main() is
        Cpt : Integer := 0;
        File_Name : String := "flash10.bin";
        F : Direct_IO.File_Type;
    begin
        Cpt := get_nb_struct(F, File_Name);
        return Cpt;
    end main;
end Principal;
		
		
		
		
