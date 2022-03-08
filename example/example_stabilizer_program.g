
LoadPackage("json");

#Define the Symmetry Group
GRP_RED := Group((12,16)(13,17)(14,18)(15,19)(45,73)(46,74)(47,75)(48,76)(49,81)(50,82)(51,83)(52,84)(53,89)(54,90)(55,91)(56,92)(57,97)(58,98)(59,99)(60,100)(61,105)(62,106)(63,107)(64,108)(65,113)(66,114)(67,115)(68,116)(69,121)(70,122)(71,123)(72,124),
(10,16)(11,17)(14,20)(15,21)(31,73)(32,74)(33,81)(34,82)(35,89)(36,90)(37,97)(38,98)(39,105)(40,106)(41,113)(42,114)(43,121)(44,122)(47,77)(48,78)(51,85)(52,86)(55,93)(56,94)(59,101)(60,102)(63,109)(64,110)(67,117)(68,118)(71,125)(72,126),
(9,16)(11,18)(13,20)(15,22)(24,73)(25,81)(26,89)(27,97)(28,105)(29,113)(30,121)(32,75)(34,83)(36,91)(38,99)(40,107)(42,115)(44,123)(46,77)(48,79)(50,85)(52,87)(54,93)(56,95)(58,101)(60,103)(62,109)(64,111)(66,117)(68,119)(70,125)(72,127),
(3,5)(4,6)(25,27)(26,28)(33,37)(34,38)(35,39)(36,40)(49,57)(50,58)(51,59)(52,60)(53,61)(54,62)(55,63)(56,64)(81,97)(82,98)(83,99)(84,100)(85,101)(86,102)(87,103)(88,104)(89,105)(90,106)(91,107)(92,108)(93,109)(94,110)(95,111)(96,112),
(2,5)(4,7)(24,27)(26,29)(31,37)(32,38)(35,41)(36,42)(45,57)(46,58)(47,59)(48,60)(53,65)(54,66)(55,67)(56,68)(73,97)(74,98)(75,99)(76,100)(77,101)(78,102)(79,103)(80,104)(89,113)(90,114)(91,115)(92,116)(93,117)(94,118)(95,119)(96,120),
(1,16)(2,73)(3,81)(4,89)(5,97)(6,105)(7,113)(8,121)(9,17)(10,18)(11,19)(12,20)(13,21)(14,22)(15,23)(24,74)(25,82)(26,90)(27,98)(28,106)(29,114)(30,122)(31,75)(32,76)(33,83)(34,84)(35,91)(36,92)(37,99)(38,100)(39,107)(40,108)(41,115)(42,116)(43,123)(44,124)(45,77)(46,78)(47,79)(48,80)(49,85)(50,86)(51,87)(52,88)(53,93)(54,94)(55,95)(56,96)(57,101)(58,102)(59,103)(60,104)(61,109)(62,110)(63,111)(64,112)(65,117)(66,118)(67,119)(68,120)(69,125)(70,126)(71,127)(72,128),
(1,5)(2,6)(3,7)(4,8)(9,27)(10,37)(11,38)(12,57)(13,58)(14,59)(15,60)(16,97)(17,98)(18,99)(19,100)(20,101)(21,102)(22,103)(23,104)(24,28)(25,29)(26,30)(31,39)(32,40)(33,41)(34,42)(35,43)(36,44)(45,61)(46,62)(47,63)(48,64)(49,65)(50,66)(51,67)(52,68)(53,69)(54,70)(55,71)(56,72)(73,105)(74,106)(75,107)(76,108)(77,109)(78,110)(79,111)(80,112)(81,113)(82,114)(83,115)(84,116)(85,117)(86,118)(87,119)(88,120)(89,121)(90,122)(91,123)(92,124)(93,125)(94,126)(95,127)(96,128));

# setup files
outfile := IO_File(Concatenation(IO_getcwd(), "/fromgap.pipe"), "w");
infile := IO_File(Concatenation(IO_getcwd(), "/togap.pipe"), "r");

str := IO_ReadLine(infile);
while str <> "break" do
        # read command from input
        if str <> "" then
            #Print("GAP READ: ", str);
            # Convert to GAP Object
            arr := JsonStringToGap(str);
            # First is the recursion level
            recursion_level := arr[1][1] + 1;
            if recursion_level < 0 then
                break;
            fi;
            # Print("Recursion Level: ", recursion_level, "\n");
            # Second is the list of Facets to test
            tarr := arr[2];
            # Third is the list of known facets
            karr := arr[3];
            # Fourth is the list of vertices on the polytope
            vertices := arr[4];
            # Caluclate the stabilizer
            GRP := GRP_RED;
            if recursion_level > 1 then
                GRP := Stabilizer(GRP_RED, vertices, OnSets);
            fi;
            
            # Response to return to RANDA
            response := [];
            # Iterate through all arrays to test
            for i in [1..Length(tarr)] do
                tpoly := tarr[i];
                equiv := 0;
                for kpoly in karr do
                    res := RepresentativeAction(GRP, tpoly, kpoly, OnSets);
                    if res <> fail then
                        equiv := 1;
                        break;
                    fi;
                od;
                if equiv = 0 then
                    Add(karr, tpoly);
                    Add(response, i-1);
                fi;
            od;
            
            # Respond
            if Length(response) = 0 then
                IO_WriteLine(outfile, "false");
            else
                IO_WriteLine(outfile, response);
            fi;
        fi;
        str := IO_ReadLine(infile); 
od;
