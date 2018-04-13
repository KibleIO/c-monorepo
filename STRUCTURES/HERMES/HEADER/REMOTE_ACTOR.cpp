
void Employ_Hermes_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}
void Unemploy_Hermes_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}


// Aliases
inline void Hermes_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Employ_Hermes_Local_Actor(employ_block, context_data);
}
inline void Hermes_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Unemploy_Hermes_Local_Actor(employ_block, context_data);
}
