//
//  ViewController.swift
//  GimBle
//
//  Created by kexuebiao on 07/16/2022.
//  Copyright (c) 2022 kexuebiao. All rights reserved.
//
import GimKit
import UIKit

class ViewController: UIViewController,GimKitDiscoveryListener {
    func onDevicesFound(devices: [GimKitDevice], code: Int) {
        print("------ %d",code)
        print(code)
        devices.forEach { GimKitDevice in
            print(GimKitDevice)
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        let type:[GimKitDeviceType] = [.EXERCISE_BIKE]
        GimKitManager.shared.startDiscovery(10,type, listener: self)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

}
