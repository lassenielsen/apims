// Recursive function that counts from 0 to 255
// Using eight boolean values as the representation of 1 byte
def X(x1: Bool, x2: Bool, x3: Bool, x4: Bool,
      x5: Bool, x6: Bool, x7: Bool, x8: Bool) =
    if (x1 and x2 and x3 and x4 and x5 and x6 and x7 and x8)
    then end
    else X(not x1,
           if x1 then not x2 else x2,
           if x1 and x2 then not x3 else x3,
           if x1 and x2 and x3 then not x4 else x4,
           if x1 and x2 and x3 and x4 then not x5 else x5,
           if x1 and x2 and x3 and x4 and x5 then not x6 else x6,
           if x1 and x2 and x3 and x4 and x5 and x6 then not x7 else x7,
           if x1 and x2 and x3 and x4 and x5 and x6 and x7 then not x7 else x7)
in X(false,false,false,false,false,false,false,false)
