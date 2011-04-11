define p_num_str
  set $i = newlines + 1
  while $i <= bytes_in_pointer
    p expression[index][$i + 1]
    set $i = $i + 1
  end
end