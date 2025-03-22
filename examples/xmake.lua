for _, file in ipairs(os.files("./*.cpp")) do
	local target_name = file:gsub("%.cpp$", "")
	target(target_name)
	set_kind("binary")
	add_files(file)
	add_deps("mnesic")
	add_packages("fmt")
end
