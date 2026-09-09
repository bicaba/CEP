-- Testing HAT using Frasy
-- Sequences
-- 1. Setup
-- 2. Power
-- 3. IO <= You must implement that sequence
-- 4. Teardown

-- IO Sequence must be run after Power, and only if it passed.

-- Advices
--- Read Frasy documentation at  https://frasy.rald.ca/
--- Read api_hat.lua to see how to send commands to the HAT 

-- Testing HAT using Frasy
-- Sequences
-- 1. Setup
-- 2. Power
-- 3. IO <= You must implement that sequence
-- 4. Teardown

-- IO Sequence must be run after Power, and only if it passed.


-- Chaque paire sera testée dans les deux sens (output/input inversés) pour couvrir les 4 GPIOs avec un minimum
-- de tests redondants.
 
Sequence("IO", function()
    local hat = Context.map.ibs.hat
   
    local pairs_to_test = {
        { a = IoEnum.PE2, b = IoEnum.PE4 },
        { a = IoEnum.PE3, b = IoEnum.PE5 },
    }

    for _, pair in ipairs(pairs_to_test) do
    -- Test 1: pin "a" en sortie en état HIGH, pin "b" en entrée
    -- doit lire ce même état à travers la résistance. Valide à la
    -- fois que "a" peut driver en output et que "b" peut lire en
    -- input.
        Test("IO " .. pair.a .. " as output, " .. pair.b .. " as input", function()
            hat:SetIoConfig(pair.b, IoMode.INPUT, IoPull.PULLDOWN)
            hat:SetIoConfig(pair.a, IoMode.OUTPUT, IoPull.NONE, IoState.HIGH)
            Expect(hat:GetIoState(pair.b), "Reads HIGH"):ToBeEqual(IoState.HIGH)

            hat:SetIoState(pair.a, IoState.LOW)
            Expect(hat:GetIoState(pair.b), "Reads LOW"):ToBeEqual(IoState.LOW)
        end)
        -- Test 2: mêmes deux pins, rôles inversés. Confirme que "b" peut
        -- aussi driver en output et que "a" peut aussi lire en input,
        -- pas seulement dans un sens.
        Test("IO " .. pair.b .. " as output, " .. pair.a .. " as input", function()
            hat:SetIoConfig(pair.a, IoMode.INPUT, IoPull.PULLDOWN)
            hat:SetIoConfig(pair.b, IoMode.OUTPUT, IoPull.NONE, IoState.HIGH)
            Expect(hat:GetIoState(pair.a), "Reads HIGH"):ToBeEqual(IoState.HIGH)

            hat:SetIoState(pair.b, IoState.LOW)
            Expect(hat:GetIoState(pair.a), "Reads LOW"):ToBeEqual(IoState.LOW)
        end)
    end
    -- Test de court-circuit sans mesure de courant
    -- disponible, on vérifie plutôt qu'une pin non connecté à une paire
    -- active ne suit pas son état. PE2/PE3 pilotent des états
    -- opposés, PE4/PE5 lisent chacune uniquement leur propre paire
    -- (PE4 lit PE2, PE5 lit PE3). Si un court-circuit existait entre les
    -- deux paires, PE4 et PE5 afficheraient le même état, ce qui ferait
    -- échouer ce test.
    Test("No cross-talk between pairs", function()
        hat:SetIoConfig(IoEnum.PE2, IoMode.OUTPUT, IoPull.NONE, IoState.HIGH)
        hat:SetIoConfig(IoEnum.PE3, IoMode.OUTPUT, IoPull.NONE, IoState.LOW)
        hat:SetIoConfig(IoEnum.PE4, IoMode.INPUT, IoPull.PULLDOWN)
        hat:SetIoConfig(IoEnum.PE5, IoMode.INPUT, IoPull.PULLUP)

        Expect(hat:GetIoState(IoEnum.PE4), "PE4 follows PE2 only"):ToBeEqual(IoState.HIGH)
        Expect(hat:GetIoState(IoEnum.PE5), "PE5 follows PE3 only"):ToBeEqual(IoState.LOW)
    end)
end)